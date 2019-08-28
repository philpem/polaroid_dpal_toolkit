/*  DEC/CMS REPLACEMENT HISTORY, Element DPSNDIMG.C */
/*  *12    3-MAY-1991 12:06:27 BROGNA "" */
/*  *11    1-APR-1991 18:02:51 BROGNA "Toolkit enhancments" */
/*  *10   18-JAN-1991 04:53:49 BROGNA "MSC 6.0x" */
/*  *9    16-NOV-1990 15:52:25 BROGNA "bug fix" */
/*  *8    16-NOV-1990 12:09:59 BROGNA "Multi-Printer support" */
/*  *7    15-OCT-1990 10:14:37 BROGNA "" */
/*  *6    10-OCT-1990 16:57:55 BROGNA "Version 2.1" */
/*  *5     5-OCT-1990 18:47:56 BROGNA "buf-cache bug fixed" */
/*  *4     3-OCT-1990 15:14:08 BROGNA "Single data structure" */
/*  *3    12-SEP-1990 11:02:35 BROGNA "ToolKit Upgrade" */
/*  *2     6-APR-1990 14:11:08 BROGNA "release 144" */
/*  *1    15-MAR-1990 11:34:09 BROGNA "new toolkit stuff" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPSNDIMG.C */
/* DPsndimg.c
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
 */

#include <string.h>

#define DPSNDIMG_C
#include "dpalette.h"
#include "dptkcent.h"

#define REQUIRED_BUFFER  (DP_CMD_PACKET_SIZE+2+ibytes)

int DP_SendImageData(DP_DATA *, int, char *, int, int) ;
static int SndImgExit(DP_DATA *, int) ;

static   char  Escape = 0x1b ;

int
DP_SendImageData(DPdata, Line, Buffer, Ibytes, Color)
   DP_DATA   *DPdata ;
   int           Line ;
   char         *Buffer ;
   int           Ibytes ;
   int           Color ;
   {
   static unsigned char tbuf[4096+(4096/64)+2+7] ;
	static int           ibytes ;
	static int           color ;

	if (DP_ERRORCLASS(DPdata) = PrInit(DP_PORT(DPdata)))
		return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;

	color = Color ;
	
   /*
    * Check for valid image compression.
    */
   if (!COMPRESSION_LEGAL(DP_IMAGE_COMPRESSION(DPdata)))
      {
      DP_ERRORCLASS(DPdata) = DPTOOLKITERR ;
      DP_ERRORNUMBER(DPdata) = TK_BAD_IMG_COMP ;
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
      }

   /*
    * If the image data is uncompressed and needs to be sent as RLE then
    * call DP_RLEncode which returns the byte length of the encoded packet.
    */
   if (DP_IMAGE_COMPRESSION(DPdata) == NON_RLE_SEND_AS_RLE)
      ibytes = DP_RLEncode(Buffer, &tbuf[7], Ibytes) ;
	else
		ibytes = Ibytes ;

   /*
    * Make sure that the Digital Palette has sufficient buffer space for this
    * data.
    * The data which may be sent includes the following:
    *   Expose color command packet + color filter arg
    *   Send Line command packet + line number + image data
    */
   if (DP_ConfirmBuffer(DPdata, REQUIRED_BUFFER, TKNOWAIT))
      return(DP_GetPrinterStatus(DPdata, INQ_ERROR)) ;
   
   /*
    * If the image data being exposed is a different color plane then let the
    * Digital Palette know.  Our variable Filter is reset by DP_StartExposure,
    * DP_TerminateExposure.  If this routine needs to return an error then it
    * will return via a call to SndImgExit which will reset Filter on error.
    */
   if (DP_FILTER_COLOR(DPdata) != Color)
      {
      strcpy(DP_COMMAND(DPdata), "EC") ;
      DP_DATA_BYTES(DPdata) = 1 ;
      DP_DATA_POINTER(DPdata) = (char *) &color ;
      if (DP_Send(DPdata, DP_TIMEOUT(DPdata)))
         return(SndImgExit(DPdata, DP_GetPrinterStatus(DPdata, INQ_ERROR))) ;
		DP_FILTER_COLOR(DPdata) = Color ;
      }   

   /*
    * Set up the command packet
    */
   tbuf[0] = 0x1b ;
   tbuf[1] = 'L' ;
   if ((DP_IMAGE_COMPRESSION(DPdata) | RLE) == RLE)
      tbuf[2] = 'E' ;
   else
      tbuf[2] = 'R' ;
   *((int *) &tbuf[3]) = ibytes + 2 ; /* image bytes + 2 byte line number */
   *((int *) &tbuf[5]) = Line ;
   
   /*
    * If this routine hasn't run-length-encoded the image data, then the
    * image data is being sent from the buffer pointer passed to this routine.
    */
   if (DP_IMAGE_COMPRESSION(DPdata))
      {
      /*
       * Send the escape byte, data length count, and image line number
       */
      DP_ERRORCLASS(DPdata) = PrSend(tbuf, 7, 1, 0, DP_TIMEOUT(DPdata)) ;
      /*
       * Send the image data
       */
      if (!DP_ERRORCLASS(DPdata))
         DP_ERRORCLASS(DPdata) = PrSend(Buffer, ibytes, 1, 0,
            DP_TIMEOUT(DPdata)) ;
      if (DP_ERRORCLASS(DPdata))
         return(SndImgExit(DPdata, DP_GetPrinterStatus(DPdata, INQ_ERROR))) ;
      }
   else
      {
      /*
       * Send the command packet and the image data
       */
      DP_ERRORCLASS(DPdata) =
         PrSend(tbuf, ibytes + 7, 1, 0, DP_TIMEOUT(DPdata)) ;
      if (DP_ERRORCLASS(DPdata))
         return(SndImgExit(DPdata, DP_GetPrinterStatus(DPdata, INQ_ERROR))) ;
      }
   
   return(DP_ERRORCLASS(DPdata)) ;
   }

static int
SndImgExit(DPdata, ErrorLevel)
   DP_DATA   *DPdata ;
   int   ErrorLevel ;
   {
   if ((ErrorLevel) && (ErrorLevel > BUFFERWARNING))
      DP_FILTER_COLOR(DPdata) = 99 ;
   return(ErrorLevel) ;
   }
