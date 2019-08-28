/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPSIMG.C */
/*  *5     3-MAY-1991 12:13:34 BROGNA "" */
/*  *4     1-APR-1991 18:03:48 BROGNA "Toolkit enhancments" */
/*  *3    18-JAN-1991 22:48:34 GRABEL "changes to run under LSC on the MAC" */
/*  *2    18-JAN-1991 04:57:45 BROGNA "MSC 6.0x" */
/*  *1     5-NOV-1990 18:08:37 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPSIMG.C */
/* scsndimg.c
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation
     
25-sep-90  vg       initial,  used dpsndimg.c from the Centronics toolkit as
                    a starting point
 4-oct-90  vg       modified for new data structures
 2-nov-90  vg       removed unused local variables
18-jan-91  vg       made tbuf an usigned char
28-mar-91  jb       substituted RLEncode() with DP_RLEncode()
 */


#define DPSNDIMG_C
#include "dpalette.h"
#include "tkhost.h"
#include "tksccmds.h"

#ifdef LSCV3            /* LightSpeed C V3.01 on the Mac..... */
#include <strings.h>
#else
#include <string.h>
#endif

int  DP_SendImageData(DP_DATA *, int, char *, unsigned int, int) ;


static   int   Error ;

int  DP_SendImageData( dp, Line, Buffer, Ibytes, Color)
DP_DATA      *dp ;
int           Line ;
char         *Buffer ;
unsigned int  Ibytes ;
int           Color ;
{
    unsigned char  tbuf[4096+(4096/64)+2] ;
    char *ibuf ;

    ibuf = (char *)tbuf+2 ;

   /*
    * If the image data is uncompressed and needs to be sent as RLE then
    * call DP_RLEncode which returns the byte length of the encoded packet.
    */
    if (DP_IMAGE_COMPRESSION( dp ) == NON_RLE_SEND_AS_RLE)
        Ibytes = DP_RLEncode(Buffer, ibuf, Ibytes) ;

    /* Make sure that the Digital Palette has sufficient buffer space for this
      data.
    */
    if (DP_ConfirmBuffer( dp , Ibytes + DP_CMD_PACKET_SIZE))
        return(DP_GetPrinterStatus( dp, INQ_ERROR)) ;

    if ( DP_IMAGE_COMPRESSION( dp ))
        memmove(ibuf, Buffer, Ibytes) ;
   
    /* Specify the line to expose and set the number of bytes to send which
       includes the two byte line number
    */
    tbuf[0] = (char)(( Line >> 8 ) & 0xFF) ;
    tbuf[1] = (char)( Line & 0xFF ) ;
    
    if ( DP_ERRORCLASS( dp ) = sc_print( tbuf, Ibytes + 2, Color ))
        return( DP_GetPrinterStatus( dp, INQ_ERROR)) ;
    
    return( DP_ERRORCLASS( dp ) ) ;
}

