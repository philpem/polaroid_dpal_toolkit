/*
	Program bwtest

	Send image to Color Palette.

*/

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#include <tiffio.h>
#include "image.h"
#include "img2dp.h"

#ifndef FALSE
#define TRUE 1
#define FALSE 0
#endif

TIFF *tif;
IMAGE *image;

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

extern IMAGE *fiopen();

main(argc,argv)
int argc;
char **argv;
{
  char filename[MAX_FILENAME];
  int iarg, i, j, do_bw, st, infile, inffd, y, color;
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
  int q1;
  long stat1[3][256];
  long stat2[3][256];

  for(color=0; color<3; color++)
  for(q1=0; q1<256; q1++)
  {
    stat1[color][q1] = 0;
    stat2[color][q1] = 0;
  }

      /* Must be file name */
      strcpy(filename, argv[1]);

#ifdef DEBUG
      printf("\nFILE NAME: %s\n\n", filename);
#endif

      swap_bw = 1;

      do_bw = 1;

      /*
	  Open the file to get the magic word:
      */

      infile = open(filename, O_RDONLY);
      if(infile == -1)
      {
	fprintf(stderr, "Could not open file: %s\n", filename);
      }

      st = read(infile, &magic, sizeof(magic));
      if(st != 2)
      {
	fprintf(stderr, "Could not read magic word.\n");
	close(infile);
      }

#ifdef DEBUG
      printf("Got magic word: %4.4x\n", magic);
#endif

      switch(magic)
      {
      case IMAGIC:
	/*
	  This is an image file.
	*/
	fprintf(stderr, "Using file: '%s'\n", filename);
	st=lseek(infile, 0, 0);
	image = fiopen(infile, "r");
	if(image == NULL)
	{
	  fprintf(stderr, "Unable to fiopen IRIS file.\n");
	  close(infile);
	  exit(-1);
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

	i_buf = (unsigned short *) malloc(width*image->zsize*sizeof(*i_buf)*4);

	for(color=0; color<=(do_bw ? 0:2); color++)
	{
	  for(y=0; y<height; y++)
	  {
	    if(do_bw)
	    {
	      if(image->zsize == 1)
	      {
		getrow(image, i_buf, height-y-1, 0);
		for(q1=0; q1<width; q1++)
		{
		  stat1[color][i_buf[q1]]++;
		}
		for(x=0; x<width; x++) d_buf[x]=i_buf[x];
	      }
	      else if(image->zsize == 3)
	      {
		for(i=0; i<3; i++)
		{
		  getrow(image, i_buf+i*width, height-y-1, i);
		  for(q1=0; q1<width; q1++)
		  {
		    stat1[i][i_buf[i*width+q1]]++;
		  }
		}
		for(x=0; x<width; x++)
		{
		  gray = ( ((long) i_buf[x])*30 +
			   ((long) i_buf[x+width])*59 +
			   ((long) i_buf[x+2*width])*11 )/100;
		  if(gray<0) gray=0;
		  if(gray>255) gray=255;
		  d_buf[x] = gray;
		}
	      } /* 3 bytes per pixel */
	    } /* bw film */
	    else
	    {
	      if(image->zsize == 1)
	      {
		getrow(image, i_buf, height-y-1, 0);
		for(q1=0; q1<width; q1++)
		{
		  stat1[color][i_buf[q1]]++;
		}
		for(x=0; x<width; x++) d_buf[x]=i_buf[x];
	      }
	      else
	      {
		getrow(image, i_buf, height-y-1, color);
		for(q1=0; q1<width; q1++)
		{
		  stat1[color][i_buf[q1]]++;
		}
		for(x=0; x<width; x++) d_buf[x]=i_buf[x];
	      }
	    } /* color film */

	    if(swap_bw)
	    {
	      for(x=0; x<width; x++) d_buf[x] = ~d_buf[x];
	    }
	    
	    for(q1=0; q1<width; q1++)
	    {
	      stat2[color][d_buf[q1]]++;
	    }
	  } /* for y */
	} /* for color */

	printf("stat1:\n");
	for(color=0; color<=2; color++)
	{
	  for(q1=0; q1<256; q1++)
	  {
	    if(stat1[color][q1]>0) printf("%3d %5d: %d\n", color, q1, stat1[color][q1]);
	  }
	}
	printf("stat2:\n");
	for(color=0; color<=2; color++)
	{
	  for(q1=0; q1<256; q1++)
	  {
	    if(stat2[color][q1]>0) printf("%3d %5d: %d\n", color, q1, stat2[color][q1]);
	  }
	}
	free(i_buf);
	break;
      otherwise:
	fprintf(stderr, "Unknown file format, magic word: %4.4x\n", magic);
	close(infile);
      } /* magic switch */

}

