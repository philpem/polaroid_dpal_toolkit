/*  DEC/CMS REPLACEMENT HISTORY, Element MKERROR.C */
/*  *8    20-JUN-1991 16:03:59 BIELEFELD "fix to msc6.0 warnings" */
/*  *7     3-MAY-1991 12:07:39 BROGNA "" */
/*  *6    18-JAN-1991 04:55:32 BROGNA "MSC 6.0x" */
/*  *5    16-JAN-1991 08:58:30 CAMPBELL_T "180 Cape work, faster in box, compile under MSC 5" */
/*  *4    11-OCT-1990 20:04:29 CAMPBELL_T "Bug fixes" */
/*  *3     8-OCT-1990 16:47:20 CAMPBELL_T "Support for Toolkit name changes" */
/*  *2     5-OCT-1990 15:10:49 CAMPBELL_T "Toolkit:: a) going to 1 struct, b) bug fixes" */
/*  *1     1-OCT-1990 21:29:17 CAMPBELL_T "toolkit example program" */
/*  DEC/CMS REPLACEMENT HISTORY, Element MKERROR.C */
/* mkerror.c

This program forces the film printer to generate a number of errors.
It should be used to test an application's ability to deal with
a variety of errors.


Usage:
    mkerror synth <n>
        Forces an error every <n> lines of image data sent.
        Application should report the error and quit.
        Turn film printer to stop error generation.
    mkerror startup
        Forces a fatal software error which restarts filmprinter.
        Application should report the error on startup.
    mkerror picrunning
        With no camera attached, this will leave a picture
        exposing.  Start application and it should ask
        user to remove film before proceeding.
    mkerror exposedfilm
        With no camera attached, this will start a picture
        and abort it.    Start application and it should ask
        user to remove film before proceeding.

28-sep-90  tc   Initial
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 8-oct-90  tc   Support toolkit change
                 SUPPRESS_BUF_WARNING --> SUPPRESS_BUFFER_WARNING
11-oct-90  tc   Added include of ../toolkit/dptkcent.h
                Changed do_a_pic() to make sure we actually exposed some
                  film.
                Bug fix in do_a_pic(), wasn't aborting the film printer
                  properly.  It was globbling up the error message from
                  the user.

Table of contents
    main
    synth_err       Handles "synth"
    startup_err     Handles "startup"
    do_a_pic        Handles "picrunning" and "exposedfilm"

    print_usage     Spews out the above stuff
    arg_check       Checks for right # of cmd line args
    
*/

#include <stdio.h>
#include <stdlib.h>
#include "dpalette.h"
#include "dptkcent.h"

#include "demosup.h"

#define LINE_LABEL  10  /* How often to label the output */

/* local function prototypes */
static void print_usage(void) ;
static void arg_check(int reqd, int got) ;
static void synth_err (DP_DATA *dp, int lines_between_errs) ;
static void startup_err(DP_DATA *dp) ;
static void do_a_pic (DP_DATA *dp, int abort_partway) ;
void strcpy (char *, char *) ;

int main( argc, argv)
    int argc ;
    char *argv[] ;
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;
    int lines_between_errs ;    /* For "synth" */

    /* Make sure there are some args */
    if ( argc < 2 )
    {   /* Error, I don't know what to do */
        print_usage() ;
        exit(1) ;
    }
    
    /* Go ahead and init the film printer.  All the tests need this */
    printf ("Initializing the film printer....\n") ;
    if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        print_err_and_exit( dp) ;
    }

    /* See which error the user wants to induce */
    if ( stricmp ( argv[1], "synth") == 0 )
    {   /* User wants to force an error every n lines.
        Dig out the number of lines from the command line */
        arg_check (argc, 3) ;   /* Make sure right # of args */

        if ( (lines_between_errs = atoi( argv[2])) == 0 )
        {   printf ("Bad argument: %s\n", argv[2] ) ;
            printf ("It should be a number of lines, e.g. 200\n") ;
            exit(1) ;
        }

        synth_err (dp, lines_between_errs) ; /* Do it */
    }
    else if ( stricmp ( argv[1], "startup") == 0 )
    {   /* User wants to force "startup" error. */
        arg_check (argc, 2) ;   /* Make sure right # of args */
        startup_err(dp) ;         /* Do it */
    }
    else if ( stricmp ( argv[1], "picrunning") == 0 )
    {   /* User wants to start a picture going */
        arg_check (argc, 2) ;   /* Make sure right # of args */
        do_a_pic(dp, FALSE) ;    /* Do it, F--> run to competion */
    }
    else if ( stricmp ( argv[1], "exposedfilm") == 0 )
    {   /* User wants to start a picture going */
        arg_check (argc, 2) ;   /* Make sure right # of args */
        do_a_pic(dp, TRUE) ; /* Do it, T--> abort in the middle */
    }
    else
    {   /* Bad command line */
        print_usage() ;
        exit(1) ;
    }
return(0);
}
/* Form Feed  */
/* synth_err

This function causes the film printer to generate a synthethic error
every "lines_between_errs" lines of image data.

28-sep-90  tc   Initial
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
				MAC, SUN

*/

static void
synth_err (dp, lines_between_errs)
    DP_DATA *dp ;
    int lines_between_errs ;    /* How often to generate error */
{
    /* Local declarations */
    int stat ;

    /* Tell 'um what to doing */
    printf ("I will force an error every %d lines.\n", lines_between_errs);

    /* Send the undocumented command: Generate Error
                <ESC> G E 2 0 lines_per_error (word)
       Note: We make calls into the Toolkit innards to do this,
            i.e. we call functions that the ISV shouldn't use.
     */

   strcpy(DP_COMMAND(dp), "GE") ;
   DP_DATA_BYTES(dp) = 2 ;
   DP_DATA_POINTER(dp) = (char *) &(lines_between_errs) ;
   if (stat = DP_Send(dp, DP_TIMEOUT(dp)))
   {   /* Got an error sending */
        DP_GetPrinterStatus(dp, INQ_ERROR) ;
        printf ("!!!! Got an error I shouldn't have gotten !!!!!\n") ;
        print_err_and_exit (dp) ;
   }


    printf ("Start your application and print an image.\n") ;
    printf ("After it has sent %d lines to the film printer,\n",
                                           lines_between_errs) ;
    printf ("it should see an error and deal with it gracefully.\n");

    return ;
}
/* Form Feed  */
/* startup_err

Forces the film printer to restart and report an error message which
the application should catch and report when started.

28-sep-90  tc   Initial
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN

*/
static void
startup_err(dp)
    DP_DATA *dp ;

{
    /* Local declarations */
    static char our_xt_data[] =     /* What we send as parameters to
                                       XT command.  See Table below */
    {   0,      /* when,  Do it now */
        1,      /* which, Divide by zero */
        0,0,    /* parm0 ignored */
        0,0     /* parm1 ignored */
    };

    /* Make sure the firmware is of the proper revision */
#define REQD_STARTUP_VERSION 160
    if ( firmware_earlier_than (dp, REQD_STARTUP_VERSION ) )
    {   printf ("Sorry, the code in the filmprinter required to run this\n");
        printf ("test wasn't implemented until Version %d of the firmware.\n",
                                REQD_STARTUP_VERSION ) ;
        printf ("Your filmprinter is %s\n", DP_ROM_VERSION(dp) ) ;
        exit(1) ;
    }

    /* Tell 'um what to doing */
    printf ("I will force the machine to restart with an error condition.\n");
    printf ("You should hear the filter wheel move.\n") ;

    /* Send the undocumented command: External Test
        Used to generate and test the runtime error recovery of the
        firmware.  We will do a divide by 0 in the film printer.

        <ESC>XT 6 0  when  which parm0 parm1     byte, byte, word, word

            when    Specs when to generate the action
                    0   At communication time, i.e. right now
                    1   At execute time, i.e. buffer the command to make
                         it happen asyncronously

            which       parm0      parm1       Description
            0           --          --          A nop, does nothing
            1           --          --          Divide by zero
            2           err_type    err_code    Generate an Async Error
            3           int #                   Generate a software interrupt

       Note: We make calls into the Toolkit innards to do this,
            i.e. we call functions that the ISV shouldn't use.
     */

   strcpy(DP_COMMAND(dp), "XT") ;
   DP_DATA_BYTES(dp) = 6 ;
   DP_DATA_POINTER(dp) = & our_xt_data[0] ;

   /* Send it, we IGNORE ERRORS */
   DP_Send(dp, DP_TIMEOUT(dp)) ;



    printf ("Start your application\n") ;
    printf ("At startup, it should report an error\n") ;

    return ;
    
    
}
/* Form Feed  */
/* do_a_pic

This starts a picture running with a Manual Camera Back.

if "abort_partway" is TRUE
    Only enough data will be sent to insure that the exposure actually
    started.  Then the image is aborted.
        
if "abort_partway" is FALSE,
    An entire image will be sent to the film printer and terminated
    normally.
        

28-sep-90  tc   Initial
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
11-oct-90  tc   Made sure really started exposed some film before ABORTing

*/

#define IMAGE_WIDTH     256
#define IMAGE_HEIGHT    256
char *color_str[] = { "Red", "Green", "Blue" } ;

static void
do_a_pic (dp, abort_partway)
    DP_DATA *dp ;
    int abort_partway ;
{
    /* Local declarations */
    char *rgb_buf, *rle_buf ;   /* Where we build image data */
    unsigned int rle_buf_length ;
    int color ;
    int y ;

    /* Check for proper camera back */
    if ( !DP_MANUALEJECT(dp) )
    {   /* Camera is not manual type */
        printf ("Wrong kind of camera back for this test\n");
        printf ("Put on a PACK or 4x5 back\n") ;
        printf ("If you haven't got one of those, just remove any\n") ;
        printf ("attached camera.\n") ;
        exit(1) ;
    }

    /* Tell 'um what we are gonna do */
    printf (
    "I am going to start an %d x %d exposure going in the film printer.\n",
        IMAGE_WIDTH, IMAGE_HEIGHT) ;
    
    /* Change some default settings */
    DP_HOR_RES(dp) = DP_LINE_LENGTH(dp) = IMAGE_WIDTH ;
    DP_VER_RES(dp) = DP_VER_HEIGHT(dp)  = IMAGE_HEIGHT ;

    /* Send the film parameters */
    printf ("Sending parameters....\n") ;
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
    printf ("Starting Exposure...\n") ;
    if ( DP_StartExposure(dp) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }

    /* Loop over each color */
    for ( color=0 ; color < NUM_USER_COLORS ; color++)
    {   
        /* Label it */
        printf ("\rSending %s             \n", color_str[color]) ;

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
    }
    /* Clean up status line */
    printf ("\r                                           \r") ;


/* Wait to make sure the film printer actually gets some film exposed. */
    printf ("Waiting for some film to be exposed....\n") ;
    do
    {   if (DP_GetPrinterStatus(dp, INQ_STATUS ) )
        {   /* Error retrieving the status */
            print_err_and_exit(dp) ;
        }
    }while ( !DP_EXPOSINGIMAGE(dp) || (DP_LINES_EXPOSED(dp) < 10) ) ;

/* All done.
    How we terminate depends on whether caller wants to abort.
    We ignore all errors, just let film printer roll */
    DP_WAITFORBUFFER(dp) = SHOW_BUFFER_WARNING ;
                                        /* Come back immediately */
    if ( abort_partway )
    {   /* We must send an abort command ourselves by dipping into the
           lower layers of the toolkit.  If we do a DP_TerminateExposure(),
           it senses and clears the error.  We want to leave it hanging around
           for the next application to find. */

        strcpy(DP_COMMAND(dp), "AB") ;
        DP_DATA_BYTES(dp) = 0 ;
        DP_DATA_POINTER(dp) = 0 ;
        DP_Send(dp, DP_TIMEOUT(dp)) ;   /* Ignore errors ! */

        printf ("I just aborted the exposure part way in process.\n") ;
        printf ("Start your application, it should immediately give you\n") ;
        printf ("A REMOVE film message\n") ;

    }
    else
    {   /* Let the picture run to completion */
        DP_TerminateExposure (dp, END_OF_EXPOSURE ) ;

        printf ("An entire exposure is in the film printer.\n") ;
        printf ("It will chug along for a while\n") ;
        printf ("Start your application, it should wait until the LED\n") ;
        printf ("quits blinking and warn you to remove the film\n") ;

    }

   return ;
}

/* Form Feed  */
/* print_usage

Prints the instructions for the screen.


28-sep-90  tc   Initial

*/

static void
print_usage()
{
    /* Local declarations */
    int line ;
    static char *help_msg[] =
    {

"This program forces the film printer to generate a number of errors.",
"It should be used to test an application's ability to deal with",
"a variety of errors.",
"Usage:",
"    mkerror synth <n>",
"        Forces an error every <n> lines of image data sent.",
"        Application should report the error and quit.",
"        Turn film printer to stop error generation.",
"    mkerror startup",
"        Forces a fatal software error which restarts filmprinter.",
"        Application should report the error on startup.",
"    mkerror picrunning",
"        With no camera attached, this will leave a picture",
"        exposing.  Start application and it should ask",
"        user to remove film before proceeding.",
"    mkerror exposedfilm",
"        With no camera attached, this will start a picture",
"        and abort it.    Start application and it should ask",
"        user to remove film before proceeding.",

""    /* Must end will EMPTY string */
} ;

    /* Loop and print */
    for ( line=0 ; *(help_msg[line]) != '\0' ; line++)
    {   printf ("%s\n", help_msg[line] ) ;
    }
    
    return ;
        
}

/* Form Feed  */
/* arg_check

Checks that reqd equals got.

If they do,    it silently returnes.
If they don't, it prints usage line and exits to OS

28-sep-90  tc   Initial

*/

static void
arg_check(reqd, got)
    int reqd ;  /* Want this many */
    int got ;   /* Got this many */
{
    if ( reqd != got )
    {   print_usage() ;
        exit(1) ;
    }
    
    return ;
}
