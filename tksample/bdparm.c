/*  DEC/CMS REPLACEMENT HISTORY, Element BDPARM.C */
/*  *6    20-JUN-1991 16:01:18 BIELEFELD "fix to msc6.0 warnings" */
/*  *5     3-MAY-1991 12:04:11 BROGNA "" */
/*  *4    18-JAN-1991 04:52:36 BROGNA "MSC 6.0x" */
/*  *3    16-OCT-1990 16:31:09 BROGNA "ABORT/Remove Film Bug fix" */
/*  *2    15-OCT-1990 16:11:19 CAMPBELL_T "Added tests for proper returns for SHOW/SUPPRES buffer warnings" */
/*  *1    11-OCT-1990 20:07:23 CAMPBELL_T "renamed, used to be badparms.c" */
/*  DEC/CMS REPLACEMENT HISTORY, Element BDPARM.C */
/* badparm.c

Usage: badparm

Sends a whole bunch of bad parameter settings to the film printer
and reports their error messages.  Doesn't take a picture.

19-sep-90  tc   Initial
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 5-oct-90  tc   Turned on the tests that were previously buggy
 8-oct-90  tc   Changed SUPRESS_BUF_WARNING --> SUPRESS_BUFFER_WARNING
11-oct-90  vg   made the bum port number > 0x17 so SCSI as well as Centronics
                Toolkits complain
11-oct-90  tc   Renamed badparms.c --> bdparm.c
15-oct-90  tc   Added checking a bunch of different error returns.
                Restructured the code a bit, moved some in-line stuff
                   into subroutines
16-oct-90  mb   More error checking, including NOERROR

Table of contents
    main
    do_bad_parms    Sends a bunch of bad parameters
    chk_exposure_returns    Checks for proper REMOVEFILM & EXPOSURE active
      chk_return            Makes sure we get the error we suppose to
      start_a_pic           Starts a picture rolling
    chk_open_files  Makes sure init code doesn't leave files open

*/

#include <stdio.h>
#include <stdlib.h>
#include "dpalette.h"
#include "demosup.h"

#define BUM_PORTNUM 0x20

/* Local function prototypes */
void do_bad_parms  (DP_DATA *dp) ;

void chk_exposure_returns (DP_DATA *dp) ;
  void start_a_pic (DP_DATA *dp, int wait_flag) ;
  void chk_return  (DP_DATA *dp, int expected_error) ;

void chk_open_files (DP_DATA *dp) ;


int main()
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;

    printf ("Sends a whole bunch of bad parameter settings to\n");
    printf ("the film printer and reports their error messages.\n") ;
    printf ("Doesn't take a picture but tries a few times.\n") ;


    /* Init it and make sure camera is proper type */
    if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) )
    {   printf ("Got an error we shouldn't have!!!\n") ;
        print_err_and_exit( dp) ;
	}

    /* Check for proper camera back */
    if ( !DP_MANUALEJECT(dp) )
    {   /* Camera is not manual type */
        printf ("\n\nWrong kind of camera back for this test\n");
        printf ("Put on a PACK or 4x5 back\n") ;
        printf ("If you haven't got one of those, just remove any\n") ;
        printf ("attached camera.\n") ;
        exit(1) ;
    }

    /* Try a bunch of bad parameters */
    do_bad_parms(dp) ;

    /* Test the proper behaviour of the toolkit with regards to an exposure
       in process on a manual camera back. */
    chk_exposure_returns(dp) ;

    /* Check that init code doesn't leave open files about */
    chk_open_files(dp) ;

    /* All done */
    printf ("\n\nAll went OK\n") ;
	return(0);
}

/* Form Feed  */
/* do_bad_parms

This sends a bunch of bad parameter settings to the film printer and
checks that the proper error return is generated.

15-oct-90  tc   Initial, copied from main() inline code

*/

#define NUM_PARMS   28

void do_bad_parms(dp)
    DP_DATA *dp ;
{
    /* Local declarations */
#define BUF_SIZE    10
    char    buf[BUF_SIZE] ; /* A place to send data from */

    int loop ;
    int do_se ;     /* T --> cmd needs Start Exposure to get error */
    int do_send ;   /* T --> cmd needs to Send Data to get error */

    /* Loop over each one we set */
    for ( loop=0 ; loop < NUM_PARMS ; loop++)
    {
            /* Init it */
            if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) )
            {   /* Error initing */
                print_err_and_exit( dp) ;
            }
            
            /* Say no SE command or need to send daata */
            do_se = do_send = FALSE ;
            printf ("%d:", loop) ;

            /* Change one paramater at a time */
            switch ( loop )
            {  
                case 0:
                    DP_PORT(dp) = BUM_PORTNUM ;
                    printf ("DP_PORT:\n  ") ;
                    break ;
                case 1:
                    DP_HOR_RES(dp) = 4097 ;
                    printf ("DP_HOR_RES:\n  ") ;
                    break ;
                 case 2 :
                    DP_VER_RES(dp) = 3900 ;
                    printf ("DP_VER_RES:\n  ") ;
                    break ;
                 case 3:
                    DP_LINE_LENGTH(dp) = DP_HOR_RES(dp) + 1 ;
                    printf ("DP_LINE_LENGTH:\n  ") ;
                    break ;
                  case 4:
                    DP_HOR_OFFSET(dp) = 15 ;
                    printf ("DP_HOR_OFFSET:\n  ") ;
                    break ;
                 case 5:
                     DP_VER_OFFSET(dp) = DP_VER_RES(dp)+4 ;
                     printf ("DP_VER_OFFSET:\n  ") ;
                     break ;
                 case 6:
                     DP_EXP_IMAGE_USAGE(dp) = 12 ;
                     do_se = TRUE ;
                     printf ("DP_EXP_IMAGE_USAGE:\n  ") ;
                     break ;
                 case 7:
                     DP_EXP_BUFFER_USAGE(dp) = 7 ;
                     do_se = TRUE ;
                     printf ("DP_EXP_BUFFER_USAGE:\n  ") ;
                     break ;
                 case 8:
                     DP_EXP_CALIBRATION(dp) = 5 ;
                     do_se = TRUE ;
                     printf ("DP_EXP_CALIBRATION:\n  ") ;
                     break ;
                 case 9:
                     DP_LIGHTEN_DARKEN(dp) = 7 ;
                     printf ("DP_LIGHTEN_DARKEN:\n  ") ;
                     break ;
                 case 10:
                     DP_EXPOSURE_TIME(dp, 1) = 49 ;
                     printf ("DP_EXPOSURE_TIME:\n  ") ;
                     break ;
                 case 11 :
                     DP_RED_EXP_TIME(dp) = 48 ;
                     printf ("DP_RED_EXP_TIME:\n  ") ;
                     break ;
                 case 12 :
                     DP_GREEN_EXP_TIME(dp) = 201 ;
                     printf ("DP_GREEN_EXP_TIME:\n  ") ;
                     break ;
                 case 13 :
                     DP_BLUE_EXP_TIME(dp) = 210 ;
                     printf ("DP_BLUE_EXP_TIME:\n  ") ;
                     break ;
                 case 14:
                     DP_LUMINANT(dp, 2) = 201 ;
                     printf ("DP_LUMINANT:\n  ") ;
                     break ;
                 case 15 :
                     DP_RED_LUMINANT(dp) = 202 ;
                     printf ("DP_RED_LUMINANT:\n  ") ;
                     break ;
                 case 16:
                     DP_GREEN_LUMINANT(dp) = 30 ;
                     printf ("DP_GREEN_LUMINANT:\n  ") ;
                     break ;
                 case 17:
                     DP_BLUE_LUMINANT(dp) = 20 ;
                     printf ("DP_BLUE_LUMINANT:\n  ") ;
                     break ;
                 case 18 :
                     DP_COLOR_BALANCE(dp, 2) = 7 ;
                     printf ("DP_COLOR_BALANCE:\n  ") ;
                     break ;
                 case 19 :
                     DP_RED_COLOR_BALANCE(dp) = 7 ;
                     printf ("DP_RED_COLOR_BALANCE:\n  ") ;
                     break ;
                 case 20:
                     DP_GREEN_COLOR_BALANCE(dp) = 7 ;
                     printf ("DP_GREEN_COLOR_BALANCE:\n  ") ;
                     break ;
                 case 21 :
                     DP_BLUE_COLOR_BALANCE(dp) = 8 ;
                     printf ("DP_BLUE_COLOR_BALANCE:\n  ") ;
                     break ;
                 case 22 :
                     DP_CAMERA_ADJUST_X(dp, 0) = 100 ;
                     printf ("DP_CAMERA_ADJUST_X:\n  ") ;
                     break ;
                 case 23:
                     DP_CAMERA_ADJUST_Y(dp, 1) = 200 ;
                     printf ("DP_CAMERA_ADJUST_Y:\n  ") ;
                     break ;
                 case 24:
                     DP_CAMERA_ADJUST_Z(dp, 2) = 200 ;
                     printf ("DP_CAMERA_ADJUST_Z:\n  ") ;
                     break ;
                 case 25:
                     DP_VER_HEIGHT(dp) = DP_VER_RES(dp)+1 ;
                     printf ("DP_VER_HEIGHT:\n  ") ;
                     break ;
                 case 26:
                     DP_BACKGROUND_MODE(dp) = 2 ;
                     printf ("DP_BACKGROUND_MODE:\n  ") ;
                     break ;
                 case 27 :
                     DP_IMAGE_COMPRESSION(dp) = 15 ;
                     do_send = TRUE ;
                     printf ("DP_IMAGE_COMPRESSION:\n  ") ;
                     break ;

                default:
                    printf ("Program Error, missing case\n") ;
                    exit(1) ;
            }

            /* Send parameters */
            DP_SendPrinterParams (dp) ;

            if (do_se || do_send )
            {   /* Need to take a picture to force error */
                    printf ("Starting Exposure...\n  ") ;
                    DP_StartExposure(dp) ;

                /* Some commands need to send data to force the error */
                if (do_send)
                {   printf ("Sending a line of data...\n  ") ;
                    DP_SendImageData(dp, 0, buf, BUF_SIZE, 0) ;
                }
            }

            if ( DP_ERRORCLASS(dp) ) 
            {   /* Got expected error */
                printf ("%s\n",  printer_err_msg( dp) ) ;
            }
            else
            {   printf ("\nUGH-OH!! Didn't get an error\n") ;
                exit(1) ;
            }
    }


/* Try to init with a bum port number */
    printf ("%d:DP_InitPrinter with bum port number\n  ", loop) ;
    if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, BUM_PORTNUM ) )
    {   /* Got expected error */
        printf ("%s\n",  printer_err_msg( dp) ) ;
    }
    else
    {   printf ("\nUGH-OH!! Didn't get an error\n") ;
        exit(1) ;
    }

/* All done */
    return ;
}

/* Form Feed  */
/* chk_exposure_returns

Starts a bunch of pictures an insures that subsequent Inits and Starts
return the proper error codes.

ASSUMES A MANUAL CAMERA BACK IS ATTACHED

15-oct-90  tc   Initial

*/

void chk_exposure_returns(dp)
    DP_DATA *dp ;
{
/* Start a picture going, this doesn't issue the TE */
    printf ("Testing SHOW_BUFFER_WARNING    TE, INIT, SE\n") ;
    start_a_pic (dp, SHOW_BUFFER_WARNING) ;
	 chk_return(dp, NOERROR) ;

    /* Terminate the picture normally */
    printf ("DP_TerminateExposure, SHOW_BUFFER_WARNING\n") ;
      DP_TerminateExposure(dp, END_OF_EXPOSURE) ;
      chk_return ( dp, EXPOSUREACTIVE) ;    /* What we expect */
  
   /* Reinit */
   printf ("DP_Init_printer, SHOW_BUFFER_WARNING\n") ;
       DP_InitPrinter(dp, SHOW_BUFFER_WARNING, FALSE) ;
       chk_return ( dp, EXPOSUREACTIVE) ;    /* What we expect */
      
    /* Try to start a new picture */
    printf ("DP_StartExposure, SHOW_BUFFER_WARNING\n") ;
        DP_StartExposure(dp) ;
        chk_return ( dp, EXPOSUREACTIVE) ;    /* What we expect */
    
    /* Try to start a new picture again */
    printf ("DP_StartExposure, SHOW_BUFFER_WARNING\n") ;
        DP_StartExposure(dp) ;
        chk_return ( dp, EXPOSUREACTIVE) ;    /* What we expect */
    
/* Abort the picture */
    printf ("DP_TerminateExposure, ABORT_EXPOSURE, SHOW_BUFFER_WARNING\n") ;
        DP_TerminateExposure(dp, ABORT_EXPOSURE) ;
        chk_return(dp, HARDWERR) ;    

/* Start a picture going, this doesn't issue the TE */
    printf ("Testing SUPPRESS_BUFFER_WARNING    TE\n") ;
    start_a_pic (dp, SUPPRESS_BUFFER_WARNING) ;
    chk_return(dp, NOERROR) ;
	 
    /* Terminate the picture normally */
    printf ("DP_TerminateExposure, SUPPRESS_BUFFER_WARNING") ;
    printf ("    Patience, this takes a while\n") ;
      DP_TerminateExposure(dp, END_OF_EXPOSURE) ;
      chk_return ( dp, REMOVEFILM) ;    /* What we expect */
  
/* Start a picture going, this doesn't issue the TE */
    printf ("Testing SUPPRESS_BUFFER_WARNING    Init\n") ;
    start_a_pic (dp, SHOW_BUFFER_WARNING) ;
    chk_return(dp, NOERROR) ;
	 
    /* Terminate the picture normally */
    printf ("DP_TerminateExposure, SHOW_BUFFER_WARNING\n") ;
      DP_TerminateExposure(dp, END_OF_EXPOSURE) ;
      chk_return ( dp, EXPOSUREACTIVE) ;    /* What we expect */
  
   /* Reinit */
   printf ("DP_Init_printer, SUPPRESS_BUFFER_WARNING") ;
   printf ("    Patience, this takes a while\n") ;
       DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) ;
       chk_return ( dp, REMOVEFILM) ;    /* What we expect */
      
/* Start a picture going, this doesn't issue the TE */
    printf ("Testing SUPPRESS_BUFFER_WARNING    SE\n") ;
    start_a_pic (dp, SHOW_BUFFER_WARNING) ;
	 chk_return(dp, NOERROR) ;

    /* Terminate the picture normally */
    printf ("DP_TerminateExposure, SHOW_BUFFER_WARNING\n") ;
      DP_TerminateExposure(dp, END_OF_EXPOSURE) ;
      chk_return ( dp, EXPOSUREACTIVE) ;    /* What we expect */
  
    /* Try to start a new picture */
    printf ("DP_StartExposure, SUPPRESS_BUFFER_WARNING") ;
    printf ("    Patience, this takes a while\n") ;
        DP_WAITFORBUFFER(dp) = SUPPRESS_BUFFER_WARNING ;
        DP_StartExposure(dp) ;
        chk_return ( dp, REMOVEFILM) ;    /* What we expect */

/* Abort the picture */
    printf ("DP_TerminateExposure, ABORT_EXPOSURE\n") ;
        DP_TerminateExposure(dp, ABORT_EXPOSURE) ;
		  chk_return ( dp, NOERROR) ;

  /* All done */
  return ;
}

/* Form Feed  */
/* chk_return

This function prints the error message sitting in "dp".

If it isn't "expected_error", that fact is announced and
exits to OS.

15-oct-90  tc   Initial


*/
void chk_return (dp, expected_error)
    DP_DATA *dp ;
    int expected_error ;
{
    /* Local declarations */
    char * err_msg_str  ;

    /* Make up an error message */
    err_msg_str = printer_err_msg( dp) ;
    if ( err_msg_str == NULL )
    {   err_msg_str = "(null)"  ;
    }


    /* Print the error message */
    printf ("  %s\n",  err_msg_str) ;
    
    /* Check that it is the proper one */
    if ( DP_ERRORCLASS(dp) != expected_error )
    {   printf ("\nUGH-OH!! Didn't get the right error\n") ;
        exit(1) ;
    }

    return ;
}

/* Form Feed  */
/* start_a_pic

This starts a picture exposing.
Some data is sent, but the Terminate Exposure command IS NOT issued.

It doesn't return until the film is actually exposed.

15-oct-90  tc   Initial
*/

#define IMAGE_WIDTH     256
#define IMAGE_HEIGHT    200

#define LINE_LABEL  10  /* How often to label the output */

void start_a_pic (dp, wait_flag)
    DP_DATA *dp ;
    int wait_flag ; /* Suppress or Show Buffer warnings */
{
    /* Local declarations */
    char *rgb_buf, *rle_buf ;   /* Where we build image data */
    unsigned int rle_buf_length ;
    int color = RED ;
    int y ;

    /* Tell what we are doing */
        printf (
    "I am going to start an %d x %d exposure going in the film printer.\n",
        IMAGE_WIDTH, IMAGE_HEIGHT) ;

    /* Init it */
    printf ("  Initing...\n") ;
    if ( DP_InitPrinter(dp, wait_flag, FALSE) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }
    
    /* Change some default settings */
    DP_HOR_RES(dp) = DP_LINE_LENGTH(dp) = IMAGE_WIDTH ;
    DP_VER_RES(dp) = DP_VER_HEIGHT(dp)  = IMAGE_HEIGHT ;

    /* Send the film parameters */
    printf ("  Sending parameters....\n") ;
    if ( DP_SendPrinterParams (dp) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }

    /* Get some memory to create an image in */
    if ( (rgb_buf = create_img_buf ( dp, 1)) == NULL )
    {   printf ("Error getting memory for image\n") ;
        exit(1) ;
    }
    
    /* Make up a constant image line */
    mk_random_pixels ( DP_LINE_LENGTH(dp), 255, 255, rgb_buf) ;

    /* RLE it once to speed the transmission */
    if ( (rle_buf = create_img_buf ( dp, 1)) == NULL )
    {   printf ("Error getting memory for image\n") ;
        exit(1) ;
    }
    DP_IMAGE_COMPRESSION(dp) = RLE ;
    rle_buf_length = DP_RLEncode( rgb_buf, rle_buf, DP_LINE_LENGTH(dp) ) ;
    
    /* Start the exposure */
    printf ("  Starting Exposure...\n") ;
    if ( DP_StartExposure(dp) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }

    printf ("  Sending RED pixels only..\n") ;

    /* Loop over each line */
    for (y=0 ; y < DP_VER_HEIGHT(dp) ; y++)
    {   
        /* Label the output */
        if ( (y % LINE_LABEL) == 0 )
        {   printf ("\r          ") ;
            printf ("\rLine %4d", y) ;
        }

        /* Send it */
        if ( DP_SendImageData(dp, y, rle_buf, rle_buf_length, color))
        {   printf ("\n") ;
            print_err_and_exit( dp) ;    /* Error */
        }
    }
    
    /* Clean up status line */
    printf ("\r                                           \r") ;

    /* All of RED is sent.  Send 1 line of GREEN to force exposure to start */
    if ( DP_SendImageData(dp, 0, rle_buf, rle_buf_length, GREEN))
    {   printf ("\n") ;
        print_err_and_exit( dp) ;    /* Error */
    }
    

    /* Wait to make sure the film printer actually gets some film exposed. */
    printf ("  Waiting for some film to be exposed....\n") ;
    do
    {   if (DP_GetPrinterStatus(dp, INQ_STATUS ) )
        {   /* Error retrieving the status */
            print_err_and_exit(dp) ;
        }
    }while ( !DP_EXPOSINGIMAGE(dp) || (DP_LINES_EXPOSED(dp) < 10) ) ;

    
    /* All done,
       NOTE that terminate exposure is NOT sent.
       Free up our memory */
    free (rle_buf) ;
    free (rgb_buf) ;

    return ;
    
}
    


/* Form Feed  */
/* chk_open_files

This re-inits the film printer a bunch of times to insure that it
properly closes any configuration files that it opens

15-oct-90  tc   Initial - copied from inline code in main()

*/

void chk_open_files(dp)
    DP_DATA *dp ;
{
    /* Local declaration */
    int loop ;

/* Make sure no open files about.
   This tests that DP_InitPrinter() doesn't leave in files open 
   when it exits.  If it does, it will use up all of the system
   file handles */
    printf ("\nGoing to do another 40 Inits of the film printer\n") ;
    for ( loop=0 ; loop < 40 ; loop++)
    {   printf ("\rInit #%d", loop) ;
        
        if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) )
        {   /* Error initing */
            print_err_and_exit( dp) ;
        }
    }
    printf ("\r                          \r") ;
    
    /* All done */
    return ;
}

