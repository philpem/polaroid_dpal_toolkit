/*  DEC/CMS REPLACEMENT HISTORY, Element CFGDUMP.C */
/*  *10    3-MAY-1991 12:04:21 BROGNA "" */
/*  *9     1-APR-1991 18:03:20 BROGNA "Toolkit enhancments" */
/*  *8    18-JAN-1991 04:52:45 BROGNA "MSC 6.0x" */
/*  *7     7-NOV-1990 14:41:36 BROGNA "SCSI toolkit compatible" */
/*  *6    10-OCT-1990 16:57:19 BROGNA "Version 2.1" */
/*  *5     3-OCT-1990 15:11:32 BROGNA "Single data structure" */
/*  *4    12-SEP-1990 11:01:10 BROGNA "ToolKit Upgrade" */
/*  *3     6-APR-1990 14:10:02 BROGNA "release 144" */
/*  *2    15-MAR-1990 17:52:46 BROGNA "Image Builders changed the config file spec" */
/*  *1    15-MAR-1990 11:32:29 BROGNA "new toolkit stuff" */
/*  DEC/CMS REPLACEMENT HISTORY, Element CFGDUMP.C */
/* CFGdump.c
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
 *
 */
#include <stdio.h>
#ifdef MSDOS
#include <conio.h>
#endif
#include "tkhost.h"

#include "dpalette.h"

#ifdef MACINTOSH_OS
static char *ConfigFileName = "dpalette.cfg" ;
#endif

#ifdef MSDOS
#include <stdlib.h>      /* for getenv() */
#endif

static   DP_DATA     dp_data ;
static   DP_DATA    *DPdata = &dp_data ;
static   char   *LDstr[7] =
               {
               "Darkest",
               "Darker",
               "Dark",
               "Normal",
               "Light",
               "Lighter",
               "Lightest"
               } ;
static   char   *ColorStr[3] =
               {
               "  Red",
               "Green",
               " Blue"
               } ;
static   int   i, j ;

main(void)
   {
   int            Size ;
   char          *ConfigDir ;
#ifdef MSDOS
   char           Drive[3] ;
   char           Path[129] ;
   char           Fname[9] ;
   char           Fext[5] ;
#endif

   /*
    * Find and read the configuration file.
    */
#if defined(MSDOS)
   ConfigDir = getenv("DPALETTE") ;
   if (!ConfigDir)
      {
      printf("Environment DPALETTE not found.") ;
      return(CONFIGERR) ;
      }
   DP_PathParse(ConfigDir, Drive, Path, Fname, Fext) ;
#endif
#ifdef SGI
   ConfigDir = (char *) getenv("DPALETTE") ;
   if (!ConfigDir)
      {
      printf("Environment DPALETTE not found.\n") ;
      return(CONFIGERR) ;
      }
#endif

#ifdef MACINTOSH_OS
	Configdir = ConfigFileName ;
#endif

   if (DP_FileOpen(DPdata, ConfigDir))
      {
      printf("Config File open error.") ;
      return(CONFIGERR) ;
      }
   Size = DP_FileRead(DPdata, (char *) DPdata, CONFIG_PARAM_BYTES) ;
   DP_FileClose(DPdata) ;
   if (Size < CONFIG_PARAM_BYTES)
      {
      printf("Config File read error.") ;
      return(CONFIGERR) ;
      }
   DP_SwapShorts(DPdata);

   printf("\n        Parallel Port: %4d", DP_PORT(DPdata)) ;
   if ((DP_PORT(DPdata) < 1) || (DP_PORT(DPdata) > 3))
      printf("\tILLEGAL") ;
   printf("\nHorizontal Resolution: %4d", DP_HOR_RES(DPdata)) ;
   if ((DP_HOR_RES(DPdata) <= 0) || (DP_HOR_RES(DPdata) > 4096))
      printf("\tILLEGAL") ;
   printf("\n    Image Line Length: %4d", DP_LINE_LENGTH(DPdata)) ;
   if ((DP_LINE_LENGTH(DPdata) <= 0) ||
      (DP_LINE_LENGTH(DPdata) > DP_HOR_RES(DPdata)))
         printf("\tILLEGAL") ;
   printf("\n    Horizontal Offset: %4d", DP_HOR_OFFSET(DPdata)) ;
   if ((DP_HOR_OFFSET(DPdata) + DP_LINE_LENGTH(DPdata)) > DP_HOR_RES(DPdata))
      printf("\tILLEGAL") ;
   printf("\n  Vertical Resolution: %4d", DP_VER_RES(DPdata)) ;
   if ((DP_VER_RES(DPdata) <= 0) || (DP_VER_RES(DPdata) > 3600))
      printf("\tILLEGAL") ;
   printf("\n      Vertical Offset: %4d", DP_VER_OFFSET(DPdata)) ;
   if (DP_VER_OFFSET(DPdata) >= DP_VER_RES(DPdata))
      printf("\tILLEGAL") ;

   printf("\n     Image Processing: %4d\t", DP_EXP_IMAGE_PROCESSING(DPdata)) ;
   switch(DP_EXP_IMAGE_PROCESSING(DPdata))
      {
      case   SE_NO_PROCESSING:
         printf("No Image Processing") ;
         break ;
      default:
         printf("ILLEGAL") ;
         break ;
      }
   
   printf("\n          Image Usage: %4d\t", DP_EXP_IMAGE_USAGE(DPdata)) ;
   switch(DP_EXP_IMAGE_USAGE(DPdata))
      {
      case   SE_ALLOW_MULTIPLE_IMAGES:
         printf("Allow Multiple Images") ;
         break ;
      case   SE_ONLY_ONE_IMAGE_IN_BUF:
         printf("Allow only one image") ;
         break ;
      default:
         printf("ILLEGAL") ;
         break ;
      }
   
   printf("\n         Buffer Usage: %4d\t", DP_EXP_BUFFER_USAGE(DPdata)) ;
   switch(DP_EXP_BUFFER_USAGE(DPdata))
      {
      case   SE_BUF_AS_RECVD:
         printf("Buffer as recieved") ;
         break ;
      case   SE_BUF_FULL_COLOR:
         printf("Buffer full color") ;
         break ;
      case   SE_BUF_FULL_IMAGE:
         printf("Buffer full image") ;
         break ;
      case   SE_BUF_OFF:
         printf("Buffer Off") ;
         break ;
      default:
         printf("ILLEGAL") ;
         break ;
      }
   
   printf("\n     Calibration Mode: %4d\t", DP_EXP_CALIBRATION(DPdata)) ;
   switch(DP_EXP_CALIBRATION(DPdata))
      {
      case   SE_NORMAL_CALIBRATION:
         printf("Normal Calibration") ;
         break ;
      case   SE_CAL_NO_CHECK:
         printf("No Calibration Check") ;
         break ;
      case   SE_NO_CAL_USE_OLD:
         printf("Use Old Calibration Values") ;
         break ;
      case   SE_NO_CAL:
         printf("No Calibration") ;
         break ;
      default:
         printf("ILLEGAL") ;
         break ;
      }
   
   if ((DP_LIGHTEN_DARKEN(DPdata) < 0) || (DP_LIGHTEN_DARKEN(DPdata) > 6))
      printf("\n  Exposure Brightness:         \tILLEGAL") ;
   else
      printf("\n  Exposure Brightness: %s",
         LDstr[DP_LIGHTEN_DARKEN(DPdata)]) ;
   
   for (i = 0 ; i < NUM_USER_COLORS ; i++)
      {
      printf("\n  %s %% Exposure Time: %4d",
         ColorStr[i], (int) DP_EXPOSURE_TIME(DPdata, i)) ;
      if ((DP_EXPOSURE_TIME(DPdata, i) < 50) ||
         (DP_EXPOSURE_TIME(DPdata, i) > 200))
            printf("\tILLEGAL") ;
      printf("\n       %s %% Luminant: %4d",
         ColorStr[i], DP_LUMINANT(DPdata, i)) ;
      if ((DP_LUMINANT(DPdata, i) < 50) || (DP_LUMINANT(DPdata, i) > 200))
         printf("\tILLEGAL") ;
      if ((DP_COLOR_BALANCE(DPdata, i) < 0) ||
         (DP_COLOR_BALANCE(DPdata, i) > 6))
            printf("\n %s ColorBalance          \tILLEGAL", ColorStr[i]) ;
      else
         printf("\n  %s Color Balance: %s",
            ColorStr[i], LDstr[DP_COLOR_BALANCE(DPdata, i)]) ;
      }

   for (i = 0 ; i < 4 ; i++)
      {
      if (DP_DEFAULT_FILM(DPdata, i) < 20)
         printf("\nFilm Default (%2u) %s",
            DP_DEFAULT_FILM(DPdata, i),
            DP_FILM_NAME(DPdata, (DP_DEFAULT_FILM(DPdata, i)))) ;
      }
   
#ifdef MSDOS
   printf("\n   Hit any key to continue...") ;
   getch() ;
#endif
   
   printf("\n%s   %s       %s      %s   %s   %s",
		"Film Type", "Film File", "Digital Palette Film Name", "X", "Y", "Z") ;
   for (i = 0 ; i < 20 ; i++)
      {
      printf("\n  %2d:     %12s       ", i, DP_FILM_TABLE_FILE(DPdata, i)) ;
      for (j = 0 ; j < 27 ; j++)
         {
         printf("%c",
            DP_FILM_NAME(DPdata, i)[j] ? DP_FILM_NAME(DPdata, i)[j] : ' ') ;
         }
      printf(" %3d %3d %3d",
         DP_CAMERA_ADJUST_X(DPdata, i),
         DP_CAMERA_ADJUST_Y(DPdata, i),
         DP_CAMERA_ADJUST_Z(DPdata, i)) ;
      }
      printf("\n");
   }
