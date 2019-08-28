/*  DEC/CMS REPLACEMENT HISTORY, Element TIMEIT.C */
/*  *6    20-JUN-1991 16:06:05 BIELEFELD "fix to msc6.0 warnings" */
/*  *5     3-MAY-1991 12:14:27 BROGNA "" */
/*  *4    18-JAN-1991 04:58:09 BROGNA "MSC 6.0x" */
/*  *3    11-OCT-1990 20:04:36 CAMPBELL_T "Bug fixes" */
/*  *2     5-OCT-1990 15:11:04 CAMPBELL_T "Toolkit:: a) going to 1 struct, b) bug fixes" */
/*  *1     1-OCT-1990 21:32:32 CAMPBELL_T "Toolkit eval/demo program" */
/*  DEC/CMS REPLACEMENT HISTORY, Element TIMEIT.C */
/* timeit.c

Usage: timeit [rle] [expose]

Sends data to film printer until the buffer fills up, announces the time,
aborts the picture.

19-sep-90  tc   Initial
27-sep-90  tc   Bug fix.  Was dividing by zero for very short times.
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
11-oct-90  tc   Changed to better manage when exposure starts

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dpalette.h"

#include "demosup.h"

#define IMAGE_WIDTH     1024     /* Size of image to expose */
#define IMAGE_HEIGHT    2048

#define COLOR   0       /* Use Red */

int main( argc, argv)
    int argc ;
    char *argv[] ;
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;

    int measure_during_expose = FALSE ; /* Default to timing with no expose */
    char compression_to_use = NON_RLE ; /* Default to send as RGB */

    time_t start_time = time(NULL) ;
    double xmit_time ;

    int arg ;
    int stat ;
    int y ;
    char *buf ;
    int xmitted_bytes_per_line ;    /* Size of each xmitted line */
    int lines_to_send ;             /* How much data will fit in buffer */

/* Tell 'um what is happening */
    printf ("Use me to time xmission of data\n");
    printf ("USAGE: timeit [RLE] [EXPOSE]\n") ;
    printf ("       RLE    Add as an argument if you want to encode and\n");
    printf ("              send the data Run Length Encoded.\n") ;
    printf ("       EXPOSE Add as an argument if you want to measure the\n");
    printf ("              transmission time while exposure is in process\n");
    printf ("I Send 1K image lines of constant data until buffer is full.\n");
    printf ("When buffer is full, I abort the picture\n") ;

/* See how they want to send data */
    for (arg=1 ; arg < argc ; arg++)
    {   if ( stricmp(argv[arg], "rle") == 0)
        {   compression_to_use = NON_RLE_SEND_AS_RLE ;
                                        /* Default to send as RGB */
        }
        else if ( stricmp(argv[arg], "expose") == 0)
        {   measure_during_expose = TRUE ;
        }
        else
        {   printf ("Bad argument: %s\n", argv[arg]) ;
            exit(1) ;
        }
    }

/* Init the film printer */
    printf ("Initializing....\n") ;
    if ( DP_InitPrinter(dp, SHOW_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        print_err_and_exit( dp ) ;
    }
    
/* Change some default settings */
    /* Change picture size */
    DP_HOR_RES(dp) = DP_LINE_LENGTH(dp) = IMAGE_WIDTH ;
    DP_VER_RES(dp) = DP_VER_HEIGHT(dp)  = IMAGE_HEIGHT ;

    /* Convert to RLE or not as required */
    DP_IMAGE_COMPRESSION(dp) = compression_to_use ;

    /* Turn off autoluma, so it doesn't interfere with timing */
    DP_EXP_CALIBRATION(dp) = SE_NO_CAL ;

    if ( measure_during_expose )
    {   /* We want the picture to start up right away */
        DP_SERVO_MODE(dp) = SERVO_OFF ;
        
    }
    else
    {   /* Only measure when exposure engine is idle */
        /* Do image at time, so we don't start exposing */
        DP_EXP_BUFFER_USAGE(dp) = SE_BUF_FULL_IMAGE ;
    }

    /* Send the film parameters */
    printf ("Sending parameters....\n") ;
    if ( DP_SendPrinterParams (dp) )
    {   print_err_and_exit( dp ) ;    /* Some kind of error */
    }
    
    /* Get some memory to create an image in */
    if ( (buf = create_img_buf ( dp, 1)) == NULL )
    {   printf ("Error getting memory for image\n") ;
        exit(1) ;
    }
    
    /* Make up a constant image line */
    mk_random_pixels ( DP_LINE_LENGTH(dp), 5, 5, buf) ;


/* Start the exposure */
    printf ("Starting Exposure...\n") ;
    if ( DP_StartExposure(dp) )
    {   print_err_and_exit( dp ) ;    /* Some kind of error */
    }

/* Send same line of data until buffer is full */
    printf ("  Actual exposure is %sIn Progress during transmission\n",
                measure_during_expose ? "" : "NOT " ) ;

    /* Figure out how many lines of data the buffer will hold */
    xmitted_bytes_per_line = (compression_to_use == NON_RLE) ?
                                7 + IMAGE_WIDTH              :
                                7 + (IMAGE_WIDTH/127) * 2    ;
    lines_to_send = (int)
        (((long) DP_FREE_BUFFER(dp) * 1024L) /
			  (long) xmitted_bytes_per_line) ;
    lines_to_send = MIN(lines_to_send, DP_VER_HEIGHT(dp) ) ;
    printf ("  I think %d lines will fit in the buffer\n", lines_to_send) ;

    printf ("Starting to send constant image data as %s pixels.....\n",
                    (compression_to_use == NON_RLE) ? "RGB" : "RLE") ;

    /* Mark the time */
    start_time = time(NULL) ;

    /* Loop until buffer is full */
    for (y=0 ; y < lines_to_send ; y++)
    {   
        /* Send it */
        stat = DP_SendImageData(dp, y, buf, DP_LINE_LENGTH(dp), COLOR) ;
        if ( stat == BUFFERWARNING)
        {   /* Buffer filled up, quit sending */
            break ;
        }
        else if (stat)
        {   print_err_and_exit( dp ) ;    /* Error */
        }
    }

    /* Stop the timer */
    xmit_time = difftime( time(NULL), start_time) ;
    printf ("Finished sending image data\n\n") ;


    /* See what the film printer thinks is in the buffer */
    if ( DP_GetPrinterStatus(dp, INQ_BUFFER ) )
    {   print_err_and_exit( dp ) ;    /* Error */
    }
    
    /* Tell um the results */
    printf ("Buffer has %dK free of %dK total\n",
        DP_FREE_BUFFER(dp), DP_TOTAL_BUFFER(dp) ) ;

    if ( compression_to_use == NON_RLE )
    {   /* RGB Data */
        printf ("Transmitted %d 1Kbyte RGB lines in %0.1f seconds\n",
                    y, xmit_time) ;
        if ( xmit_time > 0.1)
        {   printf ("Transmission Rate is %0.1f Kbytes/second\n",
                ( (double) y ) / xmit_time ) ;
        }
        else
        {   printf ("Encode/Transmision Rate is REAL, REAL fast\n" ) ;
        }
    }
    else
    {   /* RLE */
        printf ("Encoded and transmitted %d 1Kpixel CONSTANT image lines\n",
            y) ;
        printf ("    as RLE data in %0.1f seconds\n", xmit_time) ;
        if ( xmit_time > 0.1 )
        {   printf ("Encode/Transmision Rate is %0.1f Kbytes/second\n",
                ( (double) y ) / xmit_time ) ;
        }
        else
        {   printf ("Encode/Transmision Rate is REAL, REAL fast\n" ) ;
        }
    }


/* All done */
    printf ("Going to Abort the film Printer now!\n") ;            
    if ( DP_TerminateExposure (dp, ABORT_EXPOSURE))
    {   print_err_and_exit( dp ) ;    /* Some kind of error */
    }
    
    
    /* All went OK */
    printf ("Done!            \n") ;    
    return(0);

}
    
