/*  DEC/CMS REPLACEMENT HISTORY, Element DPSNDPRM.C */
/*  *17    3-MAY-1991 12:06:36 BROGNA "" */
/*  *16    1-APR-1991 18:09:56 BROGNA "Toolkit Enhancment" */
/*  *15   18-JAN-1991 04:53:57 BROGNA "MSC 6.0x" */
/*  *14   29-NOV-1990 16:58:31 BROGNA "Version 2.21" */
/*  *13   28-NOV-1990 11:51:38 BROGNA "Dual TimeOut levels ; Servo Mode Check" */
/*  *12   16-NOV-1990 15:52:36 BROGNA "bug fix" */
/*  *11   16-NOV-1990 12:10:16 BROGNA "Multi-Printer support" */
/*  *10   16-OCT-1990 16:33:49 BROGNA "ABORT/Remove Film / ERRORNUMBER Bugs Fixed" */
/*  *9    15-OCT-1990 16:11:42 CAMPBELL_T "Bug fix, abort wasn't working right" */
/*  *8    15-OCT-1990 14:09:44 BROGNA "ShowBufferWarning force mode removed" */
/*  *7    15-OCT-1990 10:14:56 BROGNA "" */
/*  *6    10-OCT-1990 16:58:02 BROGNA "Version 2.1" */
/*  *5     3-OCT-1990 15:14:16 BROGNA "Single data structure" */
/*  *4    12-SEP-1990 11:02:42 BROGNA "ToolKit Upgrade" */
/*  *3     6-APR-1990 14:11:22 BROGNA "release 144" */
/*  *2    15-MAR-1990 11:34:18 BROGNA "new toolkit stuff" */
/*  *1    28-FEB-1990 16:10:46 BROGNA "release update" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPSNDPRM.C */
/* DPsndprm.c
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.

15-oct-90  tc   Bug fix, DP_TerminateExposure ABORT wasn't right
*/

#include <string.h>

#define DPSNDPRM_C
#include "dpalette.h"
#include "dptkcent.h"

int DP_StartExposure(DP_DATA *) ;
int DP_SendPrinterParams(DP_DATA *) ;
int DP_TerminateExposure(DP_DATA *, int) ;

/****************************************************************************
 * DP_StartExposure(DPdata)
 *      Send start exposure command and args to Digital Palette.
 *
 *      Input:   Params - Pointer to structure of type POLAPARAMS.
 *               Status - Pointer to structure of type POLASTATUS.
 *      Output:  False  - No Error.
 *               True   - Error.
 *               Status structure will contain Error message and number.
 */
int
DP_StartExposure(DPdata)
   DP_DATA     *DPdata ;
   {
   if (DP_ERRORCLASS(DPdata) = PrInit(DP_PORT(DPdata)))
		return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
   
   /*
    * Set Filter to unknown value
    */
   DP_FILTER_COLOR(DPdata) = 99 ;
   
   /*
    * Check if there is an exposure in progress.  If the Digital Palette
    * is exposing an image then return a warning to the caller.
    */
   if (DP_ExposureWarning(DPdata))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Check to make sure that there is buffer space available in the Digital
    * Palette.
    */
   if (DP_ConfirmBuffer(DPdata, DP_CMD_PACKET_SIZE, TKWAIT))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Send the Start Exposure command and it's arguments.
    */
   strcpy(DP_COMMAND(DPdata), "SE") ;
   DP_DATA_BYTES(DPdata) = 4 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_EXP_IMAGE_PROCESSING(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Check to see if the Servo Mode should be turned off.
    * Send the Servo command and it's argument.
    */
   if (DP_EXP_BUFFER_USAGE(DPdata))
		DP_SERVO_MODE(DPdata) = SERVO_OFF ;
   strcpy(DP_COMMAND(DPdata), "SV") ;
   DP_DATA_BYTES(DPdata) = 1 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_SERVO_MODE(DPdata)) ;
   DP_Send(DPdata, DP_TIMEOUT(DPdata)) ;

   return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
   }

/****************************************************************************
 * DP_SendPrinterParams(DPdata)
 *      Send exposure parameters to Digital Palette.
 *      Input:   Params - Pointer to structure of type POLAPARAMS.
 *               Status - Pointer to structure of type POLASTATUS.
 */
int
DP_SendPrinterParams(DPdata)
   DP_DATA      *DPdata ;
   {
   int           color, i ;
   static char          Pdata[257] ;
	
   if (DP_ERRORCLASS(DPdata) = PrInit(DP_PORT(DPdata)))
		return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
   
   /*
    *    Make sure that the Digital Palette has sufficient buffer space to
    * accept these parameters.
    */
   if (DP_ConfirmBuffer(DPdata, PARAM_BYTES, TKWAIT))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Film Number
    */
   strcpy(DP_COMMAND(DPdata), "FN") ;
   DP_DATA_BYTES(DPdata) = 1 ;
   DP_DATA_POINTER(DPdata) = &(DP_FILMNUMBER(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Horizontal Resolution
    */
   strcpy(DP_COMMAND(DPdata), "HR") ;
   DP_DATA_BYTES(DPdata) = 2 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_HOR_RES(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Line Length
    */
   strcpy(DP_COMMAND(DPdata), "LL") ;
   DP_DATA_BYTES(DPdata) = 2 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_LINE_LENGTH(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Horizontal Offset
    */
   strcpy(DP_COMMAND(DPdata), "HO") ;
   DP_DATA_BYTES(DPdata) = 2 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_HOR_OFFSET(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Vertical Resolution
    */
   strcpy(DP_COMMAND(DPdata), "VR") ;
   DP_DATA_BYTES(DPdata) = 2 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_VER_RES(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Vertical Offset
    */
   strcpy(DP_COMMAND(DPdata), "VO") ;
   DP_DATA_BYTES(DPdata) = 2 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_VER_OFFSET(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Vertical Height
    */
   strcpy(DP_COMMAND(DPdata), "VH") ;
   DP_DATA_BYTES(DPdata) = 2 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_VER_HEIGHT(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Lighten/Darken
    */
   strcpy(DP_COMMAND(DPdata), "LD") ;
   DP_DATA_BYTES(DPdata) = 1 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_LIGHTEN_DARKEN(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   for (color = 0 ; color < NUM_USER_COLORS ; color++)
      {
      /*
       * Exposure Time
       */
      Pdata[0] = (char) color ;
      Pdata[1] = DP_EXPOSURE_TIME(DPdata, color) ;
      strcpy(DP_COMMAND(DPdata), "ET") ;
      DP_DATA_BYTES(DPdata) = 2 ;
      DP_DATA_POINTER(DPdata) = Pdata ;
      if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
         return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

      /*
       * Percent Luminant
       */
      Pdata[0] = (char) color ;
      Pdata[1] = DP_LUMINANT(DPdata, color) ;
      strcpy(DP_COMMAND(DPdata), "LU") ;
      DP_DATA_BYTES(DPdata) = 2 ;
      DP_DATA_POINTER(DPdata) = Pdata ;
      if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
         return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

      /*
       * Color Balance
       */
      Pdata[0] = (char) color ;
      Pdata[1] = DP_COLOR_BALANCE(DPdata, color) ;
      strcpy(DP_COMMAND(DPdata), "CB") ;
      DP_DATA_BYTES(DPdata) = 2 ;
      DP_DATA_POINTER(DPdata) = Pdata ;
      if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
         return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

      /*
       * Color translation Table
       */
      Pdata[0] = (char) color ;
      for (i = 0 ; i < 256 ; i++)
         Pdata[1+i] = DP_COLOR_TABLE(DPdata, color, i) ;
      strcpy(DP_COMMAND(DPdata), "CT") ;
      DP_DATA_BYTES(DPdata) = 257 ;
      DP_DATA_POINTER(DPdata) = Pdata ;
      if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
         return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
      }

   /*
    * Background Mode parameters
    */
   strcpy(DP_COMMAND(DPdata), "BM") ;
   DP_DATA_BYTES(DPdata) = 14 ;
   DP_DATA_POINTER(DPdata) = &(DP_BACKGROUND_MODE(DPdata)) ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Camera Adjust values
    */
   for (i = 0 ; i < MAX_NUM_FILMS ; i++)
      {
      Pdata[0] = (char) i ;
      Pdata[1] = DP_CAMERA_ADJUST_X(DPdata, i) ;
      Pdata[2] = DP_CAMERA_ADJUST_Y(DPdata, i) ;
      Pdata[3] = DP_CAMERA_ADJUST_Z(DPdata, i) ;
      strcpy(DP_COMMAND(DPdata), "CA") ;
      DP_DATA_BYTES(DPdata) = 4 ;
      DP_DATA_POINTER(DPdata) = Pdata ;
      if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
         return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
      }
   
   /*
    * Check to see if the Servo Mode should be turned off.
    * Send the Servo command and it's argument.
    */
   if (DP_EXP_BUFFER_USAGE(DPdata))
		DP_SERVO_MODE(DPdata) = SERVO_OFF ;
   strcpy(DP_COMMAND(DPdata), "SV") ;
   DP_DATA_BYTES(DPdata) = 1 ;
   DP_DATA_POINTER(DPdata) = (char *) &(DP_SERVO_MODE(DPdata)) ;
   DP_Send(DPdata, DP_TIMEOUT(DPdata)) ;

   return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
   }

/*
 * DP_TerminateExposure(DPdata, Method)
 *      Send start exposure command and args to Digital Palette.
 *      Input:   Params - Pointer to structure of type POLAPARAMS.
 *               Status - Pointer to structure of type POLASTATUS.
 *               Method - False = Normal, True = Abort current Exposure.
 *      Output:  False  - No Error.
 *               True   - Error.
 *      Status structure will contain Error message and number.
 */
int
DP_TerminateExposure(DPdata, Method)
   DP_DATA      *DPdata ;
   int          Method ;
   {
   if (DP_ERRORCLASS(DPdata) = PrInit(DP_PORT(DPdata)))
		return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
   
   /*
    * Set Filter to unknown value
    */
   DP_FILTER_COLOR(DPdata) = 99 ;

   /*
    * Check that the termination method is legal.
    */
   if (!TERM_METHOD_LEGAL(Method))
      {
      DP_ERRORCLASS(DPdata) = DPTOOLKITERR ;
      DP_ERRORNUMBER(DPdata) = TK_BAD_TERM_METHOD ;
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
      }

	if ((!Method) ||
		((Method == SMART_ABORT) && (!DP_SINGLEIMAGEMODE(DPdata))))
			strcpy(DP_COMMAND(DPdata), "TE") ;
	else
		strcpy(DP_COMMAND(DPdata), "AB") ;
	
   DP_DATA_BYTES(DPdata) = 0 ;
   DP_DATA_POINTER(DPdata) = 0 ;
   if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

   /*
    * Send a warning back to the caller if the film needs to be manually
    * removed from the camera.
    */
   DP_ExposureWarning(DPdata) ;

   /*
    * If servo mode has been turned off in the Digital Palette it is possible 
    * that the last call would not have detected an exposure in progress and
    * if so, would fail to return a REMOVEFILM warning to the caller.  As a
    * failsafe measure, make one final check.
    */
   if ((!DP_ERRORCLASS(DPdata)) && (DP_SINGLEIMAGEMODE(DPdata)))
      DP_ERRORCLASS(DPdata) = REMOVEFILM ;
   
   return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
   }
