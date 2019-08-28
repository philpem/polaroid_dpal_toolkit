/*  DEC/CMS REPLACEMENT HISTORY, Element DPCOMM.C */
/*  *10    3-MAY-1991 12:05:55 BROGNA "" */
/*  *9    18-JAN-1991 04:53:25 BROGNA "MSC 6.0x" */
/*  *8    29-NOV-1990 16:58:11 BROGNA "Version 2.21" */
/*  *7    28-NOV-1990 11:50:18 BROGNA "Dual TimeOut levels ; TimeOut Bug Fix" */
/*  *6    10-OCT-1990 16:57:32 BROGNA "Version 2.1" */
/*  *5     3-OCT-1990 15:13:21 BROGNA "Single data structure" */
/*  *4    12-SEP-1990 11:01:48 BROGNA "ToolKit Upgrade" */
/*  *3     6-APR-1990 14:10:45 BROGNA "release 144" */
/*  *2    15-MAR-1990 11:33:00 BROGNA "new toolkit stuff" */
/*  *1    28-FEB-1990 16:09:53 BROGNA "release update" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPCOMM.C */
/* DPCOMM.C
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation 
 *
 * Table of contents:
 *
 * DP_Send()        Sends command str and data to filmprinter
 * DP_Get()         Gets a message from the filmprinter
 * DP_ClearError()  Sends Clear Error command
 * DP_Reset()       Sends Reset Command
 *
 */

#define DPCOMM_C
#include "dpalette.h"
#include "dptkcent.h"

#define ERRORCHECK   0xffff
#define NOERRORCHECK   0

#define ESCAPE      27  /* escape character */
static char  Escape = ESCAPE ; /* deliminator character */
static int   Zero = 0 ;
static int   Error ;
static char *ResetCmd = "RE" ;
static char *ClearCmd = "CE" ;
static char *MsgCmd = "SM" ;

int        DP_Get(DP_DATA *, unsigned int) ;
int        DP_Send(DP_DATA *, unsigned int) ;
int        DP_ClearError(void) ;
int        DP_Reset(void) ;

/*
 * DP_Send(DPdata, TimeOut)
 *     sends a command string and data to the filmprinter.
 *
 */
int
DP_Send(DPdata, TimeOut)
   DP_DATA   *DPdata ;
   unsigned int  TimeOut ;
   {
   DP_ERRORCLASS(DPdata) = PrSend(&Escape, 1, ERRORCHECK, 0, TimeOut) ;

   if (!DP_ERRORCLASS(DPdata))
      DP_ERRORCLASS(DPdata) =
         PrSend(DP_COMMAND(DPdata), 2, ERRORCHECK, 0, TimeOut) ;
   if (!DP_ERRORCLASS(DPdata))
      DP_ERRORCLASS(DPdata) =
         PrSend((char *) &(DP_DATA_BYTES(DPdata)), 2, ERRORCHECK, 0,
            TimeOut) ;

   if ((!DP_ERRORCLASS(DPdata)) && (DP_DATA_BYTES(DPdata)))
      DP_ERRORCLASS(DPdata) =
         PrSend(DP_DATA_POINTER(DPdata), DP_DATA_BYTES(DPdata), ERRORCHECK, 0,
            TimeOut) ;

   /*
    * Do we need a message back?
    */
   if ((!DP_ERRORCLASS(DPdata)) && (DP_COMMAND(DPdata)[0] == 'I'))
      DP_Get(DPdata, DP_SHORT_TIMEOUT(DPdata)) ;

   return(DP_ERRORCLASS(DPdata)) ;
   }

/*
 * DP_Get(DPdata, TimeOut) stores a null terminated string from the
 *      filmprinter into buffer.
 *
 *      input:   DPdata   => pointer to DP_DATA structure
 *
 *      output:  error condition
 *                0         => no error, command and data sent successfully
 *               -1         => logic error, command or data illegal
 *               -2         => hardware error, film printer hardware error
 *               -3         => timeout error, film printer busy
 *               buffer will contain a null terminated string
 *
 */
int
DP_Get(DPdata, TimeOut)
   DP_DATA   *DPdata ;
   unsigned int  TimeOut ;
   {
   static unsigned int   eight = 8 ;
   char            *Buffer ;

   Buffer = DP_MESSAGE_POINTER(DPdata) ;
   
   do  /* following loop is done for each byte of message */
      {
		eight = 8 ;
      DP_ERRORCLASS(DPdata) = PrSend(&Escape, 1, ERRORCHECK, 0, TimeOut) ;
      if (!DP_ERRORCLASS(DPdata))
         DP_ERRORCLASS(DPdata) = PrSend(MsgCmd, 2, ERRORCHECK, 0, TimeOut) ;
      if (!DP_ERRORCLASS(DPdata))
         DP_ERRORCLASS(DPdata) =
            PrSend((char *) &eight, 2, ERRORCHECK, 0, TimeOut) ;
      if (!DP_ERRORCLASS(DPdata))
         {
         if ((DP_ERRORCLASS(DPdata) = PrGet(TimeOut)) < 0)
            {
            *Buffer = 0 ;
            return(DP_ERRORCLASS(DPdata)) ;
            }
         else
            *Buffer = (char) DP_ERRORCLASS(DPdata) ;
         }
      } while (*(Buffer++)) ;  /* continue until null byte received */

   return(DP_ERRORCLASS(DPdata)) ;
   }

/*
 * DP_ClearError()
 *   Sends a "Clear Error" command to the Digital Palette.
 *      returns 0 if the "Clear Error" command is sent.
 *      returns TIMEOUT if command not sent.
 */
int
DP_ClearError()
   {
   while (Error = PrErrors())
      {
      Error = PrSend(&Escape, 1, NOERRORCHECK, 0, STANDARD_TIMEOUT) ;
      if (!Error)
         Error = PrSend(ClearCmd, 2, NOERRORCHECK, 0, STANDARD_TIMEOUT) ;
      if (!Error)
         Error =
            PrSend((char *) &Zero, 2, NOERRORCHECK, 0, STANDARD_TIMEOUT) ;
      if (Error == TIMEOUT)
         return(TIMEOUT) ;
      }
   return(Error) ;
   }

/*
 * DP_Reset()
 *   Sends a "Reset" command to the Digital Palette.
 *      returns 0 if the "Reset" command is sent.
 *      returns TIMEOUT if command not sent.
 */
int
DP_Reset()
   {
   Error = PrSend(&Escape, 1, NOERRORCHECK, 0, STANDARD_TIMEOUT) ;
   if (!Error)
      Error = PrSend(ResetCmd, 2, NOERRORCHECK, 0, STANDARD_TIMEOUT) ;
   if (!Error)
      Error = PrSend((char *) &Zero, 2, NOERRORCHECK, 0, STANDARD_TIMEOUT) ;
   return(Error) ;
   }
