/*
	Program img2dp

	Send image to Color Palette.

*/

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#include <tiffio.h>
#include "image.h"
#include <dpalette.h>
#include "img2dp.h"

#ifndef FALSE
#define TRUE 1
#define FALSE 0
#endif

TIFF *tif;
IMAGE *image;
DP_DATA our_dp;
DP_DATA *dp = &our_dp;

#define INFFILE "img2dp.inf"
char inffilename[140];
IMG2DP_INFO img2dp_info;

#define MIN(x,y) ( (x) < (y) ? (x) : (y) )
#define MAX(x,y) ( (x) > (y) ? (x) : (y) )

static	unsigned short samplesperpixel;
static	unsigned short bitspersample;
static	unsigned short planarconfiguration;
static	unsigned short photometric;
static	unsigned short matte = 0;
static  unsigned short tf_bytesperrow;
static  unsigned long  width, height;
static int wait_for_termination=FALSE;
static int swap_bw=FALSE;

char *printer_err_msg();
void print_err_and_exit();

extern IMAGE *fiopen();

/*
	Used for update of status:

	Update file every two seconds.
*/

#define UPDATE_INTERVAL 2
static int y,color;
static time_t last_time_status = 0;
static time_t curr_time;

#define LOGFILE "/nmr1/tmp/img2dp.log"
static FILE *imglog;

main(argc,argv)
int argc;
char **argv;
{
  char filename[MAX_FILENAME];
  int use_rle = NON_RLE_SEND_AS_RLE;
  int iarg, i, j, do_bw, st, infile, inffd;
  long gray;
  unsigned short *rmap, *gmap, *bmap;
  unsigned char *crmap, *cgmap, *cbmap, *cmap;
  int x,ncolors;
  unsigned char *p1, *p2;
  unsigned char d_buf[16384], *tf_buf;
  unsigned short *i_buf;
  unsigned long width2, height2;
  unsigned short magic;
  char *configfile;
  char drive[3], path[80], fname[9], fext[4];

  /*
	Analyse parameters, set up dpalette, and start sending files.
  */

  /*
	Start up connection to palette:
  */

  imglog = fopen(LOGFILE, "a");
  fprintf(imglog, "img2dp entry.\n");
  fflush(imglog);

  if(DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE))
  {
    print_err_and_exit(dp);
  }

  if(argc == 1)
  {
    printf("img2dp\n");
    printf("       -r                Don't use RLE\n");
    printf("       -f<number>        Film number\n");
    printf("       -e<number>        Exposure: 0: Darkest, 3: Normal, 6: Lightest\n");
    printf("       -i<number>        1: invert, 0: don't invert\n");
    printf("       -w                Wait after exposure (35mm only).\n");
    printf("\n");
    printf("%s   %s       %s      %s   %s   %s\n",
		 "Film Type", "Film File", "Digital Palette Film Name", "X", "Y", "Z") ;
    for (i = 0 ; i < 20 ; i++)
    {
      printf("  %2d:     %12s       ", i, DP_FILM_TABLE_FILE(dp, i)) ;
      for (j = 0 ; j < 27 ; j++)
      {
        printf("%c", DP_FILM_NAME(dp, i)[j] ? DP_FILM_NAME(dp, i)[j] : ' ') ;
      }
      printf(" %3d %3d %3d\n",
         DP_CAMERA_ADJUST_X(dp, i),
         DP_CAMERA_ADJUST_Y(dp, i),
         DP_CAMERA_ADJUST_Z(dp, i)) ;
    }
    printf("\n");
    exit(0);
  }

  /*
	Get img2dp.inf:
  */

  configfile = DP_GetConfigPath();

  DP_PathParse(configfile, drive, path, fname, fext);

#ifdef DEBUG2
  printf("configfile: %s, drive: %s, path: %s, fname: %s, fext: %s\n",
	 configfile, drive, path, fname, fext);
#endif

  sprintf(inffilename, "%s%s", path, INFFILE);

  if(!get_info())
  {
    /*
	Could not open file - initialize to defaults.
    */

    update_img2dp();

    strcpy(img2dp_info.filename, "- none -");
    img2dp_info.color_xmit = -1; /* not xmitting */
    img2dp_info.lines_xmit = 0;
    put_info();
  }

  /*
	Loop for each argument:
  */

  for(iarg=1; iarg<argc; iarg++)
  {
    fprintf(imglog, "argument: %s\n", argv[iarg]);
    fflush(imglog);
    if(!strncmp(argv[iarg], "-r", 2))
    {
      use_rle = NON_RLE;
    }
    else if(!strncmp(argv[iarg], "-w", 2))
    {
      wait_for_termination = TRUE;
    }
    else if(!strncmp(argv[iarg], "-f", 2))
    {
      img2dp_info.filmnumber = MAX(0,MIN(19,atoi(argv[iarg]+2)));
      put_info();
    }
    else if(!strncmp(argv[iarg], "-e", 2))
    {
      img2dp_info.lighten_darken = MAX(0,MIN(6,atoi(argv[iarg]+2)));
      put_info();
    }
    else if(!strncmp(argv[iarg], "-i", 2))
    {
      img2dp_info.invert = atoi(argv[iarg]+2) == 1;
      put_info();
    }
    else
    {
      /* Must be file name */
      strcpy(filename, argv[iarg]);
      fprintf(imglog, "file: %s\n", filename);
      fflush(imglog);

#ifdef DEBUG
      printf("\nFILE NAME: %s\n\n", filename);
#endif

      /*
	    Set exposure and filmtype:
      */

      DP_LIGHTEN_DARKEN(dp) = img2dp_info.lighten_darken;
      DP_FILMNUMBER(dp) = img2dp_info.filmnumber;
      swap_bw = img2dp_info.invert;

#ifdef DEBUG
      printf("Using film: %d\n", DP_FILMNUMBER(dp));
      printf("Exposure:   %d\n", DP_LIGHTEN_DARKEN(dp));
      printf("Invert:     %s\n", swap_bw ? "Yes":"No");
#endif

      fprintf(imglog, "Using film: %d\n", DP_FILMNUMBER(dp));
      fprintf(imglog, "Exposure:   %d\n", DP_LIGHTEN_DARKEN(dp));
      fprintf(imglog, "Invert:     %s\n", swap_bw ? "Yes":"No");
      fflush(imglog);
      do_bw = DP_BLACKANDWHITEFILM(dp, DP_FILMNUMBER(dp));

#ifdef DEBUG
      printf("This film is: %s\n", (do_bw ? "B&W":"COLOR"));
#endif

      img2dp_info.wait_for_go = 0;
      img2dp_info.go_selected = 1;
      strcpy(img2dp_info.filename, filename);
      img2dp_info.color_xmit = -1; /* Not sending */
      img2dp_info.lines_xmit = 0.0;
      img2dp_info.current_color = -1; /* Not exposing */
      img2dp_info.lines_exposed = 0.0;
      put_info();

      /*
	  Open the file to get the magic word:
      */

      infile = open(filename, O_RDONLY);
      if(infile == -1)
      {
	fprintf(stderr, "Could not open file: %s\n", filename);
	continue;
      }

      st = read(infile, &magic, sizeof(magic));
      if(st != 2)
      {
	fprintf(stderr, "Could not read magic word.\n");
	close(infile);
	continue;
      }

#ifdef DEBUG
      printf("Got magic word: %4.4x\n", magic);
#endif

      switch(magic)
      {
      case TIFF_BIGENDIAN:
      case TIFF_LITTLEENDIAN:
	st=lseek(infile, 0, 0);
	tif = TIFFFdOpen(infile, filename, "r");
	if(tif == NULL)
	{
	  /*
	    Skipping this file - could not open.
	  */
	  fprintf(stderr, "Could not open file: '%s'\n", filename);
	  continue;
	}
#ifdef DEBUG
	printf("Starting on file: %s\n", filename);
#endif

	/*
	    Get information from TIFF file:
	*/

	tf_bytesperrow = TIFFScanlineSize(tif);
	TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
	TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarconfiguration);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
	TIFFGetField(tif, TIFFTAG_MATTEING, &matte);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
#ifdef DEBUG
	printf("Information from TIFF file:\n\n");
	printf("Bytes per row:         %d\n", tf_bytesperrow);
	printf("Scanlinesize:          %d\n", TIFFScanlineSize(tif));
	printf("Bits per sample:       %d\n", bitspersample);
	printf("Samples per pixel:     %d\n", samplesperpixel);
	printf("Planar Configuration:  %d\n", planarconfiguration);
	printf("Photometric:           %d\n", photometric);
	printf("Matte:                 %d\n", matte);
	printf("Width:                 %d\n", width);
	printf("Height:                %d\n", height);
	printf("\n");
#endif

	/*
	    Now let's see what we have:
	*/

	if(matte)
	{
	  fprintf(stderr, "Cannot handle matte.\n");
	  TIFFClose(tif);
	  continue;
	}
	switch(photometric)
	{
	case PHOTOMETRIC_RGB:
	  if(bitspersample != 8)
	  {
	    fprintf(stderr, "Must have 8 bits per sample for RGB.\n");
	    TIFFClose(tif);
	    continue;
	  }
	  if(samplesperpixel != 3)
	  {
	    fprintf(stderr, "Must have 3 samples per pixel for RGB.\n");
	    TIFFClose(tif);
	    continue;
	  }
	  break;
	case PHOTOMETRIC_PALETTE:
	  break;
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_MINISWHITE:
	  break;
	}
	if(planarconfiguration != PLANARCONFIG_CONTIG)
	{
	  fprintf(stderr, "Planar configuration must be Contig.\n");
	  TIFFClose(tif);
	  continue;
	}

	/*
	    Calculating width and height:

	    First, we simply set the values to what they are specified to:
	*/
#ifdef DEBUG
	printf("x-aspect: %d, y-aspect: %d\n", DP_X_ASPECT(dp,DP_FILMNUMBER(dp)), DP_Y_ASPECT(dp,DP_FILMNUMBER(dp)));
#endif

	DP_LINE_LENGTH(dp) = width;
	DP_VER_HEIGHT(dp) = height;
	DP_HOR_RES(dp) = width;
	DP_VER_RES(dp) = height;
	DP_HOR_OFFSET(dp) = 0;
	DP_VER_OFFSET(dp) = 0;

	width2 = (double) height * (double) DP_X_ASPECT(dp,DP_FILMNUMBER(dp)) / 
				   (double) DP_Y_ASPECT(dp,DP_FILMNUMBER(dp));
	height2 = (double) width * (double) DP_Y_ASPECT(dp,DP_FILMNUMBER(dp)) / 
				   (double) DP_X_ASPECT(dp,DP_FILMNUMBER(dp));
	if(height2>3600)
	{
	  printf("height2 adjusted from %d to 3600.\n", height2);
	  height2=3600;
	}
	if(width2>4096)
	{
	  printf("width2 adjusted from %d to 4096.\n", width2);
	  width2 = 4096;
        }

	if(width2 > width)
	{
	  DP_HOR_RES(dp) = width2;
	  DP_HOR_OFFSET(dp) = (width2-width)/2;
	}
	else if(height2 > height)
	{
	  DP_VER_RES(dp) = height2;
	  DP_VER_OFFSET(dp) = (height2-height)/2;
	}

	if(DP_HOR_RES(dp)>4096)
	{
	}

	if(DP_VER_RES(dp)>3600)
	{
	}

#ifdef DEBUG
	printf("DP_LINE_LENGTH:  %d\n", DP_LINE_LENGTH(dp));
	printf("DP_HOR_RES:      %d\n", DP_HOR_RES(dp));
	printf("DP_HOR_OFFSET:   %d\n", DP_HOR_OFFSET(dp));
	printf("DP_VER_HEIGHT:   %d\n", DP_VER_HEIGHT(dp));
	printf("DP_VER_RES:      %d\n", DP_VER_RES(dp));
	printf("DP_VER_OFFSET:   %d\n", DP_VER_OFFSET(dp));
#endif

	DP_IMAGE_COMPRESSION(dp) = use_rle;

	/*
		Buffer whole color before exposure:
	*/

	DP_EXP_BUFFER_USAGE(dp) = SE_BUF_FULL_COLOR;

	/*
	    Send the parameters:
	*/

	if(DP_SendPrinterParams(dp))
	{
	  print_err_and_exit(dp);
	}

	/*
	    Start exposure:
	*/

	if(DP_StartExposure(dp))
	{
	  print_err_and_exit(dp);
	}

	switch(photometric)
	{
	case PHOTOMETRIC_RGB:
	  /*
	    This must be 3 bytes per pixel.
	  */
	  if(do_bw)
	  {
	    /*
	       combine the colors into b&w:
	    */
	    tf_buf = (unsigned char *) malloc(TIFFScanlineSize(tif));
	    color = 0;
	    for(y=0; y<height; y++)
	    {
	      update_status_if();
	      if(TIFFReadScanline(tif, tf_buf, y, 0) < 0)
	      {
		break;
	      }
	      for(x=0; x<width; x++)
	      {
		gray = ( ((long) tf_buf[x*3  ])*30 +
			 ((long) tf_buf[x*3+1])*59 +
			 ((long) tf_buf[x*3+2])*11 )/100;
		if(gray<0) gray=0;
		if(gray>255) gray=255;
		d_buf[x] = gray;
	      }
#ifdef DEBUG2
	      if(y%10==0)printf("color = %d, y = %d\n", 0, y);
#endif
	      if(swap_bw)
	      {
		for(x=0; x<width; x++) d_buf[x] = ~d_buf[x];
	      }
	      if(DP_SendImageData(dp, y, d_buf, width, color)) print_err_and_exit(dp);
	    } /* for y */
	    free(tf_buf);
	  } /* RGB to b&w film */
	  else
	  {
	    /*
		    RGB to color film:
	    */
	    tf_buf = (unsigned char *) malloc(TIFFScanlineSize(tif));
	    for(color=0; color<NUM_USER_COLORS; color++)
	    {
	      for(y=0; y<height; y++)
	      {
		update_status_if();
		if(TIFFReadScanline(tif, tf_buf, y, 0) < 0)
		{
		  break;
		}
		for(x=0; x<width; x++)
		{
		  d_buf[x] = tf_buf[x*3+color];
		}
#ifdef DEBUG2
		if(y%10==0)printf("color = %d, y = %d\n", color, y);
#endif
		if(swap_bw)
		{
		  for(x=0; x<width; x++) d_buf[x] = ~d_buf[x];
		}
		if(DP_SendImageData(dp, y, d_buf, width, color)) print_err_and_exit(dp);
	      } /* for y */
	    } /* for color */
	    free(tf_buf);
	  }
	  break;
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
	  /*
	    Black-and-White stuff:
	  */
	  tf_buf = (unsigned char *) malloc(TIFFScanlineSize(tif));
	  for(color=0; color<=(do_bw ? 0:NUM_USER_COLORS-1); color++)
	  {
	    for(y=0; y<height; y++)
	    {
	      update_status_if();
	      if(TIFFReadScanline(tif, tf_buf, y, 0) < 0)
	      {
#ifdef DEBUG
		printf("Hm, strange thing at line %d, y = %d\n", __LINE__, y);
#endif
		break;
	      }
#ifdef DEBUG2
	      if(y%10==0)printf("color = %d, y = %d\n", color, y);
#endif
	      /*
		    Unpack, according to bitspersample:
	      */
	      p1 = d_buf;
	      p2 = tf_buf;
	      switch(bitspersample)
	      {
	      case 8:
		for(x=0; x<width; x++) *p1++ = *p2++;
		break;
	      case 4:
		for(x=0; x<width/2+1; x++)
		{
		  *p1++ = ((*p2  >>4)&0x0f)*255/15;
		  *p1++ = ((*p2++>>0)&0x0f)*255/15;
		}
		break;
	      case 2:
		for(x=0; x<width/4+1; x++)
		{
		  *p1++ = ((*p2  >>6)&0x03)*255/3;
		  *p1++ = ((*p2  >>4)&0x03)*255/3;
		  *p1++ = ((*p2  >>2)&0x03)*255/3;
		  *p1++ = ((*p2++>>0)&0x03)*255/3;
		}
		break;
	      case 1:
		for(x=0; x<width/8+1; x++)
		{
		  *p1++ = ((*p2  >>7)&0x01)*255;
		  *p1++ = ((*p2  >>6)&0x01)*255;
		  *p1++ = ((*p2  >>5)&0x01)*255;
		  *p1++ = ((*p2  >>4)&0x01)*255;
		  *p1++ = ((*p2  >>3)&0x01)*255;
		  *p1++ = ((*p2  >>2)&0x01)*255;
		  *p1++ = ((*p2  >>1)&0x01)*255;
		  *p1++ = ((*p2++>>0)&0x01)*255;
		}
		break;
	      }

	      /*
		    Reverse, if MINISWHITE
	      */
	      if(photometric == PHOTOMETRIC_MINISWHITE)
	      {
		for(x=0; x<width; x++) d_buf[x] = ~d_buf[x];
	      }

	      if(swap_bw)
	      {
		for(x=0; x<width; x++) d_buf[x] = ~d_buf[x];
	      }
	      if(DP_SendImageData(dp, y, d_buf, width, color)) print_err_and_exit(dp);
	    } /* for y */
	  } /* for color */
	  free(tf_buf);
	  break;
	case PHOTOMETRIC_PALETTE:
	  /*
	    Use palette lookup:
	  */

	  if(!TIFFGetField(tif, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap))
	  {
	    fprintf(stderr, "PHOTOMETIC_PALETTE file without palette.\n");
	    TIFFClose(tif);
	    continue;
	  }

#define CVTCMAP(x) (((x)*255L)/65535L)

	  ncolors = 1<<bitspersample;
	  crmap = (unsigned char *) malloc(ncolors);
	  cgmap = (unsigned char *) malloc(ncolors);
	  cbmap = (unsigned char *) malloc(ncolors);

	  if(do_bw)
	  {
	    /*
		    This is simply done at the colormap level:
	    */
	    for(i=0; i<ncolors; i++)
	    {
	      gray = ( ((long) rmap[i])*30 +
		       ((long) gmap[i])*59 +
		       ((long) bmap[i])*11 )/100;
	      if(gray<0) gray=0;
	      if(gray>65535) gray=65535;
	      crmap[i] = CVTCMAP(((unsigned short) gray));
	    }
	  }
	  else
	  {
	    for(i=0; i<ncolors; i++)
	    {
	      crmap[i] = CVTCMAP(rmap[i]);
	      cgmap[i] = CVTCMAP(gmap[i]);
	      cbmap[i] = CVTCMAP(bmap[i]);
	    }
	  }

	  tf_buf = (unsigned char *) malloc(TIFFScanlineSize(tif));
	  for(color=0; color<=(do_bw ? 0:NUM_USER_COLORS-1); color++)
	  {
	    switch(color)
	    {
	    case 0:
	      cmap = crmap;
	      break;
	    case 1:
	      cmap = cgmap;
	      break;
	    case 2:
	      cmap = cbmap;
	      break;
	    }
	    for(y=0; y<height; y++)
	    {
	      update_status_if();
	      if(TIFFReadScanline(tif, tf_buf, y, 0) < 0)
	      {
		break;
	      }
	      /*
		    Unpack, according to bitspersample:
	      */
	      p1 = d_buf;
	      p2 = tf_buf;
	      switch(bitspersample)
	      {
	      case 8:
		for(x=0; x<width; x++) *p1++ = cmap[*p2++];
		break;
	      case 4:
		for(x=0; x<width/2+1; x++)
		{
		  *p1++ = cmap[((*p2  >>4)&0x0f)];
		  *p1++ = cmap[((*p2++>>0)&0x0f)];
		}
		break;
	      case 2:
		for(x=0; x<width/4+1; x++)
		{
		  *p1++ = cmap[((*p2  >>6)&0x03)];
		  *p1++ = cmap[((*p2  >>4)&0x03)];
		  *p1++ = cmap[((*p2  >>2)&0x03)];
		  *p1++ = cmap[((*p2++>>0)&0x03)];
		}
		break;
	      case 1:
		for(x=0; x<width/8+1; x++)
		{
		  *p1++ = cmap[((*p2  >>7)&0x01)];
		  *p1++ = cmap[((*p2  >>6)&0x01)];
		  *p1++ = cmap[((*p2  >>5)&0x01)];
		  *p1++ = cmap[((*p2  >>4)&0x01)];
		  *p1++ = cmap[((*p2  >>3)&0x01)];
		  *p1++ = cmap[((*p2  >>2)&0x01)];
		  *p1++ = cmap[((*p2  >>1)&0x01)];
		  *p1++ = cmap[((*p2++>>0)&0x01)];
		}
		break;
	      }
#ifdef DEBUG2
	      if(y%10==0)printf("color = %d, y = %d\n", color, y);
#endif
	      if(swap_bw)
	      {
		for(x=0; x<width; x++) d_buf[x] = ~d_buf[x];
	      }
	      if(DP_SendImageData(dp, y, d_buf, width, color)) print_err_and_exit(dp);
	    } /* for y */
	  } /* for color */
	  free(tf_buf);
	  free(cbmap);
	  free(cgmap);
	  free(crmap);
	  break;
	}
	TIFFClose(tif);
	break;
      case IMAGIC:
	/*
	  This is an image file.
	*/
	st=lseek(infile, 0, 0);
	image = fiopen(infile, "r");
	if(image == NULL)
	{
	  fprintf(stderr, "Unable to fiopen IRIS file.\n");
	  close(infile);
	  continue;
	}
	
	/*
	  Test the types.
	*/

	width = image->xsize;
	height = image->ysize;

#ifdef DEBUG
	printf("IRIS file: xsize: %d, ysize: %d, zsize: %d, dim: %d\n", image->xsize, image->ysize, image->zsize, image->dim);
#endif

	/*
	    Calculating width and height:

	    First, we simply set the values to what they are specified to:
	*/
#ifdef DEBUG
	printf("x-aspect: %d, y-aspect: %d\n", DP_X_ASPECT(dp,DP_FILMNUMBER(dp)), DP_Y_ASPECT(dp,DP_FILMNUMBER(dp)));
#endif

	DP_LINE_LENGTH(dp) = width;
	DP_VER_HEIGHT(dp) = height;
	DP_HOR_RES(dp) = width;
	DP_VER_RES(dp) = height;
	DP_HOR_OFFSET(dp) = 0;
	DP_VER_OFFSET(dp) = 0;

	width2 = (double) height * (double) DP_X_ASPECT(dp,DP_FILMNUMBER(dp)) / (double) DP_Y_ASPECT(dp,DP_FILMNUMBER(dp));
	height2 = (double) width * (double) DP_Y_ASPECT(dp,DP_FILMNUMBER(dp)) / (double) DP_X_ASPECT(dp,DP_FILMNUMBER(dp));

	if(height2>3600)
	{
	  printf("height2 adjusted from %d to 3600.\n", height2);
	  height2=3600;
	}
	if(width2>4096)
	{
	  printf("width2 adjusted from %d to 4096.\n", width2);
	  width2 = 4096;
        }

	if(width2 > width)
	{
	  DP_HOR_RES(dp) = width2;
	  DP_HOR_OFFSET(dp) = (width2-width)/2;
	}
	else if(height2 > height)
	{
	  DP_VER_RES(dp) = height2;
	  DP_VER_OFFSET(dp) = (height2-height)/2;
	}

#ifdef DEBUG
	printf("DP_LINE_LENGTH:  %d\n", DP_LINE_LENGTH(dp));
	printf("DP_HOR_RES:      %d\n", DP_HOR_RES(dp));
	printf("DP_HOR_OFFSET:   %d\n", DP_HOR_OFFSET(dp));
	printf("DP_VER_HEIGHT:   %d\n", DP_VER_HEIGHT(dp));
	printf("DP_VER_RES:      %d\n", DP_VER_RES(dp));
	printf("DP_VER_OFFSET:   %d\n", DP_VER_OFFSET(dp));
	printf("image z-size:    %d\n", image->zsize);
#endif

	DP_IMAGE_COMPRESSION(dp) = use_rle;
	DP_EXP_BUFFER_USAGE(dp) = SE_BUF_FULL_COLOR;

	/*
	    Send the parameters:
	*/

	if(DP_SendPrinterParams(dp))
	{
	  print_err_and_exit(dp);
	}

	/*
	    Start exposure:
	*/

	if(DP_StartExposure(dp))
	{
	  print_err_and_exit(dp);
	}

	i_buf = (unsigned short *) malloc(width*image->zsize*sizeof(*i_buf)*4);

	for(color=0; color<=(do_bw ? 0:NUM_USER_COLORS-1); color++)
	{
	  for(y=0; y<height; y++)
	  {
#ifdef DEBUG2
	    if(y%10==0)printf("color = %d, y = %d\n", color, y);
#endif
	    update_status_if();
	    if(do_bw)
	    {
	      if(image->zsize == 1)
	      {
		getrow(image, i_buf, height-y-1, 0);
		for(x=0; x<width; x++) d_buf[x]=i_buf[x];
	      }
	      else if(image->zsize >= 3)
	      {
		for(i=0; i<3; i++)
		getrow(image, i_buf+i*width, height-y-1, i);
		for(x=0; x<width; x++)
		{
		  gray = ( ((long) i_buf[x])*30 +
			   ((long) i_buf[x+width])*59 +
			   ((long) i_buf[x+2*width])*11 )/100;
		  if(gray<0) gray=0;
		  if(gray>255) gray=255;
		  d_buf[x] = gray;
		}
	      } /* 3 bytes per pixel (or more) */
	    } /* bw film */
	    else
	    {
	      if(image->zsize == 1)
	      {
		getrow(image, i_buf, height-y-1, 0);
		for(x=0; x<width; x++) d_buf[x]=i_buf[x];
	      }
	      else
	      {
		getrow(image, i_buf, height-y-1, color);
		for(x=0; x<width; x++) d_buf[x]=i_buf[x];
	      }
	    } /* color film */

	    if(swap_bw)
	    {
	      for(x=0; x<width; x++) d_buf[x] = ~d_buf[x];
	    }
	    if(DP_SendImageData(dp, y, d_buf, width, color)) print_err_and_exit(dp);
	  } /* for y */
	} /* for color */
	free(i_buf);
	break;
      otherwise:
	fprintf(stderr, "Unknown file format, magic word: %4.4x\n", magic);
	close(infile);
      } /* magic switch */

      color = -1; /* finished transmitting */

      /*
	If packfilm, we must wait until the user removes the
	film and signals this by pressing the "Next Picture"
	button in the dpc window.
      */

      if(DP_CAMERABACK(dp) == CAMERA_PACK)
      {
	img2dp_info.current_color = -1;
	img2dp_info.lines_exposed = 1.0;
	img2dp_info.color_xmit = -1;
	img2dp_info.lines_xmit = 0.0;
	strcpy(img2dp_info.filename, "- none -");
	put_info();
	st=DP_TerminateExposure(dp, END_OF_EXPOSURE);
	if(st!=REMOVEFILM) print_err_and_exit(dp);
	img2dp_info.wait_for_go = 1;
	img2dp_info.go_selected = 0;
	put_info();
	do
	{
	  sleep(UPDATE_INTERVAL);
	  get_info();
	  /*
		  The following may be changed between pictures:
	  */
	  DP_FILMNUMBER(dp) = img2dp_info.filmnumber;
	  DP_LIGHTEN_DARKEN(dp) = img2dp_info.lighten_darken;
	  swap_bw = img2dp_info.invert;
	  update_status_if();
	} while(!img2dp_info.go_selected);
      }
      else
      {
	if(DP_TerminateExposure(dp, END_OF_EXPOSURE)) print_err_and_exit(dp);
	if(!wait_for_termination)
	{
	  img2dp_info.current_color = -1;
	  img2dp_info.lines_exposed = 1.0;
	}
	img2dp_info.color_xmit = -1;
	img2dp_info.lines_xmit = 0.0;
	strcpy(img2dp_info.filename, "- none -");
	put_info();
      }
    } /* if argument is file name */
  } /* for each argument */
  if(DP_CAMERABACK(dp) != CAMERA_PACK)
  {
    if(wait_for_termination)
    {
      fprintf(stderr, "Waiting for termination...\n");
      do
      {
	update_status_if();
	sleep(UPDATE_INTERVAL);
      } while (DP_EXPOSINGIMAGE(dp));
    }
  }
  fprintf(stderr, "End of img2dp.\n");
  fprintf(imglog, "End of img2dp.\n");
  fclose(imglog);
}

void print_err_and_exit(dp)
DP_DATA *dp;
{
  char *err_msg;

  err_msg = printer_err_msg(dp);
  fprintf(stderr, "\n%s\n", err_msg);
  exit(1);
}

char *printer_err_msg ( dp )
    DP_DATA *dp ;
{
    /* Local declarations */
    static char    err_msg[1024] ;  /* What we return */
    char           working_msg[512] ; /* Part of err_msg built here */

   fprintf(imglog, "printer_err_msg called.\n");
   fflush(imglog);

   /* Determine nature of error */
   if ( DP_ERROR(dp) )
   {    strcpy(err_msg, "ERR: ") ;
   }
   else if ( DP_WARNING(dp) )
   {    strcpy(err_msg, "WARNING: ") ;
   }
   else
   {    /* No error */
        strcpy(err_msg, "OKAY: ") ;
   }

   /* Label the class of error,
      We init err_msg here */       
   switch(DP_ERRORCLASS(dp))
   {
         case LOGICERR:
            strcat(err_msg, "LE") ;
            break ;
         case HARDWERR:
            strcat(err_msg, "HE") ;
            break ;
         case TIMEOUT:
            strcat(err_msg, "TO") ;
            break ;
         case DISCONNECTED:
            strcat(err_msg, "DC") ;
            break ;
         case UNKNOWNPORT:
            strcat(err_msg, "UP") ;
            break ;
         case CONFIGERR:
            strcat(err_msg, "CE") ;
            break ;
        case DPTOOLKITERR:
            strcat(err_msg, "TK") ;
            break ;
        case BUFFERWARNING:
            strcat(err_msg, "BF") ;
            break ;
        case EXPOSUREACTIVE:
            strcat(err_msg, "EA") ;
            break ;
        case REMOVEFILM:
            strcat(err_msg, "RF") ;
            break ;
        case NOERROR:
            break ;

         default:
           sprintf(err_msg, "Unknown Error Condition, Class=%d",
               DP_ERRORCLASS(dp) ) ;
           break ;
         }


      /* If applicable, give um the error number.
          We build this message in working_msg, and tack it on */
      if (DP_ERRORNUMBER(dp))
      {   sprintf (working_msg, ", #%d ", DP_ERRORNUMBER(dp)) ;
          strcat (err_msg, working_msg) ;
      }

      /* and give 'um a full error message */
      if( DP_ERRORMESSAGE(dp)[0] )
      {   strcat (err_msg, ": ") ;
          strcat (err_msg, DP_ERRORMESSAGE(dp)) ;
      }

      /* All done */
      fprintf(imglog, "Error text: %s\n", err_msg);
      fflush(imglog);
      return err_msg ;
    
}

update_img2dp()
{
  int i;

  /*
	Get status from the box:
  */

  if(DP_GetPrinterStatus(dp, INQ_STATUS))
  {
    print_err_and_exit(dp);
  }

  img2dp_info.background_mode = DP_BACKGROUND_MODE(dp);
  img2dp_info.background_pixel_value = DP_BACKGROUND_PIX_VALUE(dp);
  img2dp_info.blue_color_balance = DP_BLUE_COLOR_BALANCE(dp);
  img2dp_info.blue_exp_time = DP_BLUE_EXP_TIME(dp);
  img2dp_info.blue_luminant = DP_BLUE_LUMINANT(dp);
  img2dp_info.filmnumber = DP_FILMNUMBER(dp);
  img2dp_info.green_color_balance = DP_GREEN_COLOR_BALANCE(dp);
  img2dp_info.green_exp_time = DP_GREEN_EXP_TIME(dp);
  img2dp_info.green_luminant = DP_GREEN_LUMINANT(dp);
  img2dp_info.imagesinqueue = DP_IMAGESINQUEUE(dp);
  img2dp_info.lighten_darken = DP_LIGHTEN_DARKEN(dp);
  img2dp_info.invert = swap_bw;
  if(DP_EXPOSINGIMAGE(dp))
  {
    img2dp_info.current_color = DP_CURRENTCOLOR(dp);
    img2dp_info.lines_exposed = (float) DP_LINES_EXPOSED(dp)/(float) DP_VER_HEIGHT(dp);
#ifdef DEBUG2
    printf("Exposing %d, %d out of %d: %g\n", DP_CURRENTCOLOR(dp), DP_LINES_EXPOSED(dp), DP_VER_HEIGHT(dp), img2dp_info.lines_exposed);
#endif
  }
  else
  {
    img2dp_info.current_color = -1;
    img2dp_info.lines_exposed = 0.0;
#ifdef DEBUG2
    printf("- not exposing\n");
#endif
  }
  for(i=0; i<3; i++)
  {
    img2dp_info.lower_left_color[i] = DP_LOWER_LEFT_COLOR(dp, i);
    img2dp_info.lower_right_color[i] = DP_LOWER_RIGHT_COLOR(dp, i);
    img2dp_info.upper_left_color[i] = DP_UPPER_LEFT_COLOR(dp, i);
    img2dp_info.upper_right_color[i] = DP_UPPER_RIGHT_COLOR(dp, i);
  }
  img2dp_info.red_color_balance = DP_RED_COLOR_BALANCE(dp);
  img2dp_info.red_exp_time = DP_RED_EXP_TIME(dp);
  img2dp_info.red_luminant = DP_RED_LUMINANT(dp);

  img2dp_info.color_xmit = color;
  img2dp_info.lines_xmit = (float) y/(float) height;
}

/*
	Get the info file:
*/
int get_info()
{
  int retry;
  int fh, st;

  for(retry=0; retry<10; retry++)
  {
    fh = open(inffilename, O_RDONLY);
    if(fh == -1) {sleep(1); continue;}

    st=read(fh, &img2dp_info, sizeof(img2dp_info));
    if(st != sizeof(img2dp_info))
    {
      close(fh);
      sleep(1);
      continue;
    }
    close(fh);
    return TRUE;
  }

#ifdef DEBUG
  printf("Could not read file: %s\n", inffilename);
#endif
  return FALSE;
}

/*
	Put the info file:
*/
put_info()
{
  int retry;
  int fh, st;

  umask(0000);

  for(retry=0; retry<10; retry++)
  {
    fh = open(inffilename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fh == -1) {sleep(1); continue;}

    st=write(fh, &img2dp_info, sizeof(img2dp_info));
    if(st != sizeof(img2dp_info))
    {
      close(fh);
      sleep(1);
      continue;
    }
    close(fh);
    return;
  }

  fprintf(stderr, "Could not write file: %s\n", inffilename);
  exit(-1);
}

update_status()
{
  last_time_status = curr_time;
  update_img2dp();
  put_info();
}

update_status_if()
{
  curr_time = time(NULL);

  if(difftime(curr_time, last_time_status) >= (double) UPDATE_INTERVAL)
  {
    update_status();
  }
}


