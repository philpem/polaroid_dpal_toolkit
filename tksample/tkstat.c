/*  DEC/CMS REPLACEMENT HISTORY, Element TKSTAT.C */
/*  *7     3-MAY-1991 12:15:31 BROGNA "" */
/*  *6    18-JAN-1991 04:58:35 BROGNA "MSC 6.0x" */
/*  *5     5-NOV-1990 17:38:32 GRABEL "made port reporting SCSI aware" */
/*  *4    11-OCT-1990 20:05:26 CAMPBELL_T "Added toolkit version printout" */
/*  *3     8-OCT-1990 16:48:32 CAMPBELL_T "Support for Toolkit name changes" */
/*  *2     5-OCT-1990 15:11:13 CAMPBELL_T "Toolkit:: a) going to 1 struct, b) bug fixes" */
/*  *1     1-OCT-1990 21:32:04 CAMPBELL_T "Toolkit eval/demo program" */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKSTAT.C */
/* tkstat.c
 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
	 
Usage: tkstat

Retrieves and prints all the status information


19-sep-90  tc   Initial
27-sep-90  tc   Changed to show buffer warnings, so will print
                 if there is an exposure already in the box
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 4-oct-90  vg   missed one of the De-ANSI's
 8-oct-90  tc   Changed DP_COMMANDBUFFERSIZE to DP_ON_BOARD_RAM
 8-oct-90  tc   Support toolkit change
                 SUPPRESS_BUF_WARNING --> SUPPRESS_BUFFER_WARNING
 11-oct-90  tc  Added printout of toolkit version number
29-oct-90  vg   Added print-out of SCSI ID if using SCSI toolkit
*/

#include <stdio.h>
#include "dpalette.h"
#include "demosup.h"

char *color_str[] = { "Red", "Green", "Blue", "Nothing" } ;


int main(void)
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;

    printf ("TKSTAT, Retrieves and prints all the status information.\n\n") ;

    /* Init it */
    if ( DP_InitPrinter(dp, SHOW_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        printf ("Got an error Initing the film printer!\n") ;
        printf ("%s\n", printer_err_msg(dp) ) ;
        printf ("We will still try to print the status\n") ;
    }
            
    /* Get all status information */
    if ( DP_GetPrinterStatus(dp, ALL_INQUIRIES) )
    {   /* Error getting status */
        printf ("Error getting status!\n") ;
        printf ("%s\n", printer_err_msg(dp) ) ;
    }
    
/* Start printing it */
    /* Results of the init */
    printf ("The filmprinter is %son-line and %sinitialized.\n",
        DP_DPALETTE_ONLINE(dp)        ? "" : "NOT ",
        DP_DPALETTE_INITIALIZED(dp)   ? "" : "NOT " ) ;
    if ( DP_PORT(dp) < DP_SCSIID_OFFSET )
        printf ("It is attached to LPT%d.\n", DP_PORT(dp) ) ;
    else
        printf ("It is attached to SCSI ID %d\n",
                 DP_PORT(dp) - DP_SCSIID_OFFSET ) ;
             
    printf ("It is called %s.\n\n", DP_DEVICE_NAME(dp) ) ;


    /* Configuration stuff */
    printf ("The Toolkit is: %s\n", DP_TOOLKIT_VERSION(dp) ) ;
    printf ("The firmware is %s.\n", DP_ROM_VERSION(dp) ) ;
    printf ("Maximum Horizontal Resolution is %dK Pixels.\n",
                    DP_MAXHORIZONTALRES(dp));
    printf ("There is %sa SCSI interface attached.\n\n",
                    DP_SCSIPORT(dp) ? "" : "NOT ") ;

    /* Buffer stuff */
    printf ("The installed memory size is %dK bytes.\n",
            DP_ON_BOARD_RAM(dp) ) ;
    printf ("Command buffer size is %dK bytes, with %dK bytes free.\n",
        DP_TOTAL_BUFFER(dp), DP_FREE_BUFFER(dp) ) ;
    printf ("There IS %sBuffer Space Available.\n\n",
            DP_BUFFERAVAILABLE(dp) ? "" : "NOT " ) ;
        
    /* Exposure stuff */
    printf ("You ARE %sexposing an image%s.\n", 
            DP_EXPOSINGIMAGE(dp)   ? "" : "NOT ",
            DP_SINGLEIMAGEMODE(dp) ? " in Single Image Mode" : "" ) ;
    printf ("There are %d buffered images in the buffer.\n",
            DP_IMAGESINQUEUE(dp) ) ;
    printf ("You are exposing line %d of %s\n",
            DP_LINES_EXPOSED(dp), color_str[  DP_CURRENTCOLOR(dp) ] ) ;
    printf ("Film printers says: %s\n\n", DP_EXPOSURE_STATUS(dp) ) ;
    
    /* Talk about the camera */
    printf ("There is a %s camera attached, type %d\n",
        DP_CAMERADESCRIPTION(dp), DP_CAMERABACK(dp) ) ;
    if (DP_AUTOADVANCE(dp))
        printf ("It can automatically advance the film\n") ; 
    else if (DP_MANUALEJECT(dp))
        printf ("You must manually remove the film\n") ;
    else
        printf ("CAMERA MACROS ARE BUSTED, neither manual or auto\n") ;

	 return(-1);
}
