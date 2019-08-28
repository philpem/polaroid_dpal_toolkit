/*  DEC/CMS REPLACEMENT HISTORY, Element DPTKUTIL.C */
/*  *2     3-MAY-1991 12:07:02 BROGNA "" */
/*  *1     1-APR-1991 18:12:29 BROGNA "Toolkit Enhancment" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPTKUTIL.C */
/* DPTKUTIL.C
 * Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
 *
 */
#include <stdio.h>
#include <stdlib.h>      /* for getenv() */
#include "dpalette.h"

char       * DP_GetConfigPath(void) ;
void         DP_Sleep(int) ;
int          DP_FileOpen(DP_DATA *, char *) ;
int          DP_FileRead(DP_DATA *, char *, int) ;
int          DP_FileClose(DP_DATA *) ;
unsigned int DP_RLEncode(char *, char *, int) ;
void         DP_PathParse(char *, char *, char *, char *, char *) ;
void	     DP_SwapShorts(DP_DATA *) ;

/****************************************************************************
 * DP_GetConfigPath() calls the C function getenv() to obtain the environment
 *    variable DPALETTE from DOS.  If the environment variable is unavailable,
 *    for instance if your application does not retain the PSP, you should
 *    modify this routine to get the path/filename of the configuration file
 *    from some other source.
 */
char *
DP_GetConfigPath()
	{
	  char *p;
	  static char *defp="/indy2/pcdisk/pccommon/dpalette/dpalette.cfg";
	  p=getenv("DPALETTE");
	  if(p)
	    return p;
	  else
	    return defp;
	}

/****************************************************************************
 * DP_Sleep(num) will "sleep" for num seconds.  This routine calls the routine
 *   WaitForTicks() which uses the DOS time of day interrupt to count clock
 *   ticks until it is time to "wake up."  If this is inappropriate for your
 *   application, for example running under multi-tasking environment, you
 *   should modify this routine.
 */
void
DP_Sleep(secs)
	int	secs ;
   {
   /*
    * The time of day clock "ticks" at a freqency of 18.2 ticks per second.
    */
	sleep(secs) ;
	}

/****************************************************************************
 * DP_FileOpen(dp, name) uses ANSI C function fopen() for read access and
 *   stores the file pointer in structure dp as macro DP_FILE_POINTER(dp)
 *   which is a union member of type FILE.
 *
 *   If you modify this routine to open the file with a low-level open which
 *   uses int file handles, then store the handle in macro DP_FILE_HANDLE(dp).
 *   example:
 *     DP_FILE_HANDLE(dp) = open(FileName, OS_SPECIFIC_READ_MODE) ; 
 */
int
DP_FileOpen(dp, FileName)
   DP_DATA *dp ;
   char    *FileName ;
   {
	if ((DP_FILE_POINTER(dp) = fopen(FileName, "rb")) == NULL)
   	return(-1) ;
	else
		return(0) ;
	}

/****************************************************************************
 * DP_FileRead(dp, Buf, NumBytes) uses ANSI C fgetc() to read NumBytes bytes
 *   into Buf from the file described by file pointer DP_FILE_POINTER(dp).
 */
int
DP_FileRead(dp, Buf, NumBytes)
   DP_DATA   *dp ;
   char      *Buf ;
   int        NumBytes ;
	{
	int   BytesRead ;
	int   c ;
	
	BytesRead = 0 ;

   while (((c = fgetc(DP_FILE_POINTER(dp))) != EOF) && BytesRead <= NumBytes)
      *(Buf+(BytesRead++)) = (char) (c & 0xff) ;

   return(BytesRead) ;
	}

/****************************************************************************
 * DP_FileClose(dp) uses ANSI C fclose() to close the file described by file
 *    pointer DP_FILE_POINTER(dp).
 */
int
DP_FileClose(dp)
   DP_DATA *dp ;
	{
	return(fclose(DP_FILE_POINTER(dp))) ;
	}

/****************************************************************************
 * DP_RLEncode(dp) calls RLEncode() in file RLENCODE.C which is shipped with
 *    this toolkit.  If you have another run-length encoder, plug it in here.
 */
unsigned int
DP_RLEncode(in, out, NumBytes)
	char  *in ;
   char  *out ;
	int    NumBytes ;
	{
	return(RLEncode(in, out, NumBytes)) ;
	}

/****************************************************************************
 * DP_PathParse(dp) calls PathParse() in file PATHPARS.C which is shipped with
 *    this toolkit.  PathParse() emulates MicroSoft's _splitpath() function.
 *    If you want to the Toolkit's parser, plug it in here.
 */
void
DP_PathParse(FPath, Drive, Path, Fname, Fext)
   char   *FPath ;
   char   *Drive ;
   char   *Path ;
   char   *Fname ;
   char   *Fext ;
	{
	PathParse(FPath, Drive, Path, Fname, Fext) ;
	}

void DP_SwapShorts(dp)
DP_DATA *dp;
{
  dp->iPort = SSWAP(dp->iPort);
  dp->iHorRes = SSWAP(dp->iHorRes);
  dp->iVerRes = SSWAP(dp->iVerRes);
  dp->iLineLength = SSWAP(dp->iLineLength);
  dp->iHorOff = SSWAP(dp->iHorOff);
  dp->iVerOff = SSWAP(dp->iVerOff);
  dp->iVertHeight = SSWAP(dp->iVertHeight);
}

