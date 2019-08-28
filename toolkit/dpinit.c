/*  DEC/CMS REPLACEMENT HISTORY, Element DPINIT.C */
/*  *18    3-MAY-1991 12:06:09 BROGNA "" */
/*  *17    1-APR-1991 18:03:08 BROGNA "Toolkit enhancments" */
/*  *16   25-MAR-1991 16:08:44 BROGNA "Bug Fix" */
/*  *15   18-JAN-1991 04:53:32 BROGNA "MSC 6.0x" */
/*  *14   29-NOV-1990 16:58:17 BROGNA "Version 2.21" */
/*  *13   28-NOV-1990 11:50:35 BROGNA "Dual TimeOut levels" */
/*  *12   16-OCT-1990 16:33:06 BROGNA "ABORT/Remove Film / ERRORNUMBER Bugs Fixed" */
/*  *11   15-OCT-1990 10:13:43 BROGNA "" */
/*  *10   10-OCT-1990 16:57:38 BROGNA "Version 2.1" */
/*  *9     5-OCT-1990 18:46:39 BROGNA "single image mode handler" */
/*  *8     3-OCT-1990 15:13:43 BROGNA "Single data structure" */
/*  *7    14-SEP-1990 14:01:46 BROGNA "Null Pointer Assignment vanquished" */
/*  *6    12-SEP-1990 11:02:09 BROGNA "ToolKit Upgrade" */
/*  *5     6-APR-1990 14:11:58 BROGNA "release 144" */
/*  *4    23-MAR-1990 15:48:45 BROGNA "" */
/*  *3    15-MAR-1990 17:52:29 BROGNA "Image Builders changed the config file spec" */
/*  *2    15-MAR-1990 11:33:49 BROGNA "new toolkit stuff" */
/*  *1    28-FEB-1990 16:10:23 BROGNA "release update" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPINIT.C */
/* DPINIT.C
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
 *
 */

#include <string.h>

#define DPINIT_C
#include "dpalette.h"
#include "dptkcent.h"

int DP_InitPrinter(DP_DATA *, int, int) ;
int DP_DownLoadFilms(DP_DATA *, char *, int) ;

#define  NUM_ESCS_TO_SEND    4200  /* More than a lines worth */

int
DP_InitPrinter(DPdata, BufferWait, PortOverRide)
   DP_DATA      *DPdata ;  /* pointer to the toolkit data structure */
   int          BufferWait ; /* buffer availability warning suppression */
   int          PortOverRide ; /* overides cfg port if not zero */
   {
   int           FilmSize ;
   int           Color, i, c ;
   char          Pdata[4200] ;
   char          Escape = 0x1b ;
   char         *ConfigDir ;
   char          Drive[3] ;
   char          Path[129] ;
   char          Fname[9] ;
   char          Fext[5] ;
   char          FilmFile[129] ;

   /*
    * Clear status information
    */
   DP_STATUS_BYTE_ZERO(DPdata) = 0 ;
   DP_STATUS_BYTE_ONE(DPdata) = 0 ;
   DP_LINES_EXPOSED(DPdata) = 0 ;
   DP_EXPOSURE_STATUS(DPdata)[0] = '\0' ;
   DP_OPTION_BYTE_ZERO(DPdata) = 0 ;
   DP_ROM_VERSION(DPdata)[0] = '\0' ;
   for (i = 0 ; i < MAX_NUM_FILMS ; i++)
      for (c = 0 ; c < ASPECT_RATIO_SIZE ; c++)
         DP_ASPECT_MESSAGE(DPdata, i)[0] = '\0' ;
   DP_FREE_BUFFER(DPdata) = 0 ;
   DP_TOTAL_BUFFER(DPdata) = 0 ;
   DP_CAMERA_CODE(DPdata) = 0 ;
   DP_CAMERADESCRIPTION(DPdata)[0] = '\0' ;
   DP_ERRORNUMBER(DPdata) = 0 ;
   DP_ERRORMESSAGE(DPdata)[0] = '\0' ;
   DP_ERRORCLASS(DPdata) = 0 ;
   DP_DPALETTE_INITIALIZED(DPdata) = 0 ;
   DP_DPALETTE_ONLINE(DPdata) = 0 ;
   strcpy(DP_TOOLKIT_VERSION(DPdata), TOOLKIT_VERSION) ;
   
   /*
    * Get the path/name of the configuration file from the environment
    *    variable DPALETTE.
    *    DP_GetConfigPath() is located in dptkutil.c and may be modified
    *    if the DOS environment is unavailable.
    */
   ConfigDir = DP_GetConfigPath() ;
   if (!ConfigDir)
      {
      DP_ERRORCLASS(DPdata) = CONFIGERR ;
      DP_ERRORNUMBER(DPdata) = CFG_NO_ENVIRONMENT ;
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
      }
   
   /*
    * Parse the path and filename out of the environment string and open the
    *   file for read.
    */
   DP_PathParse(ConfigDir, Drive, Path, Fname, Fext) ;
   if (DP_FileOpen(DPdata, ConfigDir))
      {
      DP_ERRORCLASS(DPdata) = CONFIGERR ;
      DP_ERRORNUMBER(DPdata) = CFG_FILE_OPEN_ERROR ;
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
      }

   /*
    * Read the configuration data into the DPdata structure.  Make sure
    *   that all the data was read in.
    */
   FilmSize = DP_FileRead(DPdata, (char *) DPdata, CONFIG_PARAM_BYTES) ;
   DP_FileClose(DPdata) ;
   if (FilmSize < CONFIG_PARAM_BYTES)
      {
      DP_ERRORCLASS(DPdata) = CONFIGERR ;
      DP_ERRORNUMBER(DPdata) = CFG_FILE_READ_ERROR ;
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
      }

   /*
    * Default Vertical Height
    *   assumed to be centered in the vertical dimension
    */
   DP_VER_HEIGHT(DPdata) = DP_VER_RES(DPdata) - (2 * DP_VER_OFFSET(DPdata)) ;
   
   /*
    * Default Servo Method
    */
   DP_SERVO_MODE(DPdata) = DEFAULT_SERVO ;
   
   /*
    * Set up a default Color table
    */
   for (Color = 0 ; Color < 3 ; Color++)
      for (i = 0 ; i < 256 ; i++)
         DP_COLOR_TABLE(DPdata, Color, i) = (char) i ;

   /*
    * Default Background Mode = off
    * Default Background Values
    */
   DP_BACKGROUND_MODE(DPdata) = 0 ;
   DP_BACKGROUND_PIX_VALUE(DPdata) = 0 ;
   for (Color = 0 ; Color < 3 ; Color++)
      {
      DP_UPPER_LEFT_COLOR(DPdata, Color) = 0 ;
      DP_UPPER_RIGHT_COLOR(DPdata, Color) = 0 ;
      DP_LOWER_LEFT_COLOR(DPdata, Color) = 0 ;
      DP_LOWER_RIGHT_COLOR(DPdata, Color) = 0 ;
      }

   /*
    * Default Image Compression
    */
   DP_IMAGE_COMPRESSION(DPdata) = NON_RLE_SEND_AS_RLE ;

   /*
    * How do we want to handle a full buffer in the Digital Palette
    *   TRUE  : Wait until sufficient buffer space becomes available
    *   FALSE : Don't wait.  Return a Warning to the caller; let the
    *           caller decide whether to wait or not.
    */
   DP_WAITFORBUFFER(DPdata) = (char) BufferWait ;
   
   /*
    * Set up our communications port.
    * Make sure it's known to the system.
    * Check to see if we need to override the configuration port.
    */
   if (PortOverRide)
      DP_PORT(DPdata) = PortOverRide ;

   if (DP_ERRORCLASS(DPdata) = PrInit(DP_PORT(DPdata)))
      {
      DP_GetPrinterStatus(DPdata, INQ_ERROR) ;
      if ((DP_ERRORCLASS(DPdata) == UNKNOWNPORT) && (!PortOverRide))
         {
         DP_ERRORCLASS(DPdata) = CONFIGERR ;
         DP_ERRORNUMBER(DPdata) = CFG_PORT_ERROR ;
         DP_GetPrinterStatus(DPdata, INQ_ERROR) ;
         }
      return(DP_ERRORCLASS(DPdata)) ;
      }

   /*
    * TimeOut limits for Initialization communications with the
    * Digital Palette.
    */
   DP_SHORT_TIMEOUT(DPdata) = STANDARD_TIMEOUT ;
   DP_LONG_TIMEOUT(DPdata) = LONG_TIMEOUT ;

   /*
    * Send 4200 ESC's to the Digital Palette in case the prior program
    * was interrupted in the middle of sending a long line of data to
    * the Digital Palette.  We specify a short TimeOut duration to avoid a
    * lengthy delay in case the Digital Palette is unavailable and if there
    * is an error we stop.  We don't bother checking the type of error now
    * because if one exists it will be dealt with in the next step.
    */
   for (i = 0 ; (i < NUM_ESCS_TO_SEND) && (!DP_ERRORCLASS(DPdata)) ; i++)
      DP_ERRORCLASS(DPdata) = PrSend(&Escape, 1, 1, 0, SHORT_TIMEOUT) ;

   /*
    * Check for any existing errors or Power up Diagnostic errors in the
    * Digital Palette.  On existing errors, the error line will already be
    * set, but for power up diagnostic errors the error line will be set only
    * after the first 5 byte command packet.  We'll send down an Inquire Error
    * command.  This will ensure that the error line is set in either case.
    * If there is no error then we should quietly proceed.
    */
   if (!DP_ERRORCLASS(DPdata))
      {
      strcpy(DP_COMMAND(DPdata), "IE") ;
      DP_DATA_BYTES(DPdata) = 0 ;
      DP_DATA_POINTER(DPdata) = (char *) 0 ;
      DP_MESSAGE_POINTER(DPdata) = DP_ERRORMESSAGE(DPdata) - 1 ;
      DP_ERRORCLASS(DPdata) = DP_Send(DPdata, SHORT_TIMEOUT) ;
      }
   
   /*
    * There are a few errors which can be present at this time which the
    * user should be alerted to.  Any other errors may be safely cleared
    * and ignored since the next step in initializing the Digital Palette
    * is to issue the Reset Command.
    */
   DP_GetPrinterStatus(DPdata, INQ_ERROR) ;
   if (DP_ERRORCLASS(DPdata))
      {
      if ((DP_ERRORNUMBER(DPdata) == 2)   /* film previously exposed */
         || (DP_ERRORNUMBER(DPdata) == 3) /* filter wheel jam */
         || (DP_ERRORNUMBER(DPdata) == 24) /* busy on SCSI line */
         || (DP_ERRORNUMBER(DPdata) == 30) /* internal software error */
         || (DP_ERRORNUMBER(DPdata) == 31) /* other serious hardware error */
         || (DP_ERRORCLASS(DPdata) == TIMEOUT) /* can't communicate */
         )
         return(DP_ERRORCLASS(DPdata)) ;
      }

   /*
    * See if we have to return any exposure warnings.
    */
   if (DP_ExposureWarning(DPdata))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Reset the Digital Palette.  This clears any existing errors and puts
    * the Digital Palette in a machine default state.
    */
   if ((DP_ERRORCLASS(DPdata) = DP_Reset()))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Send all the Configuration File downloadable Data to the Digital
    * Palette.
    *
    * Downloadable film tables and camera adjustments.
    */
   for (i = 0 ; i < MAX_NUM_FILMS ; i++)
      {
      if (DP_FILM_TABLE_FILE(DPdata, i)[0])
         {
         strcpy(FilmFile, Drive) ;
         strcat(FilmFile, Path) ;
         strcat(FilmFile, DP_FILM_TABLE_FILE(DPdata, i)) ;
         if (DP_FileOpen(DPdata, FilmFile))
            {
            DP_ERRORCLASS(DPdata) = CONFIGERR ;
            DP_ERRORNUMBER(DPdata) = CFG_FILM_OPEN_ERROR ;
            DP_GetPrinterStatus(DPdata, INQ_ERROR) ;
            strcat(DP_ERRORMESSAGE(DPdata), DP_FILM_TABLE_FILE(DPdata, i)) ;
            return(DP_ERRORCLASS(DPdata)) ;
            }
         Pdata[0] = (char) i ;
         FilmSize = DP_FileRead(DPdata, &Pdata[1], MAX_FILM_FILE_SIZE) ;
         DP_FileClose(DPdata) ;
         /*
          * Make sure that the Digital Palette has sufficient buffer space
          * for this film table.  If there are no errors or warnings at this
          * point it is OK to send the film table to the Digital Palette.
          */
         if (!DP_ConfirmBuffer(DPdata, FilmSize + DP_CMD_PACKET_SIZE, TKWAIT))
            {
				if (DP_DownLoadFilms(DPdata, Pdata, FilmSize + 1))
					return (DP_ERRORCLASS(DPdata)) ;
            }
         if (DP_ERRORCLASS(DPdata))
            return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
         }
      }

   /*
    * Check on any known errors in the Digital Palette.
    * Get the current Exposure status of the Digital Palette.
    * Get the Digital Palette Options.
    * Get the Digital Camera back information.
    * Find out how much buffer space is available.
    */
   DP_GetPrinterStatus(DPdata, ALL_INQUIRIES) ;
   if (DP_ERRORCLASS(DPdata))
      return(DP_ERRORCLASS(DPdata)) ;

   /*
    * Update the film printer.  If the Digital Palette reports a logic error,
    * then we assume it is because of a bad parameter from the config file
    * so we'll set DP_ERRORCLASS(DPdata) to reflect a configuration file
    * error.  DP_ERRORMESSAGE(DPdata) will still point to a Digital Palette
    * message describing the error.
    */
   DP_SendPrinterParams(DPdata) ;
   if (DP_ERRORCLASS(DPdata) == LOGICERR)
      DP_ERRORCLASS(DPdata) = CONFIGERR ;

   /*
    * Update the Error message.
    */
   DP_GetPrinterStatus(DPdata, INQ_ERROR) ;
   
   if (DP_ERROR(DPdata))
      DP_DPALETTE_INITIALIZED(DPdata) = 0 ;
   else
      DP_DPALETTE_INITIALIZED(DPdata) = 1 ;

   return(DP_ERRORCLASS(DPdata)) ;
   }

int
DP_DownLoadFilms(DP_DATA * DPdata, char * FilmBuffer, int nBytes)
   {
   strcpy(DP_COMMAND(DPdata), "FT") ;
   DP_DATA_BYTES(DPdata) = (int) nBytes ;
   DP_DATA_POINTER(DPdata) = FilmBuffer ;
   if (DP_Send(DPdata, DP_SHORT_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
	}
