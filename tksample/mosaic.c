/*  DEC/CMS REPLACEMENT HISTORY, Element MOSAIC.C */
/*  *5    20-JUN-1991 16:04:39 BIELEFELD "fix to msc6.0 warnings" */
/*  *4     3-MAY-1991 12:07:54 BROGNA "" */
/*  *3    18-JAN-1991 04:55:39 BROGNA "MSC 6.0x" */
/*  *2    16-JAN-1991 08:59:08 CAMPBELL_T "180 Cape work, faster in box, compile under MSC 5" */
/*  *1    11-OCT-1990 20:08:03 CAMPBELL_T "renamed, used to be mosaic.c  " */
/*  DEC/CMS REPLACEMENT HISTORY, Element MOSAIC.C */
/* mosaic.c

Usage: mosaic [-ffilmnumber]

An program built on top of the toolkit which shoots a bunch of different
pictures on the same piece of film.

 5-oct-90  tc   Initial
11-oct-90  tc   Renamed mosiac.c --> mosaic.c !A Typo!

Table of contents
    main
    mk_pixels   Creates data to send to film printer

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dpalette.h"

#include "demosup.h"

#define MAX_ARGS    2           /* Max legal value of argc */
#define PRINT_USAGE (printf ("Usage: mosaic [-fFILMNUMBER]\n") )

/* Make the code easy to read, this erases the status line */
#define ERASE_STATUS_LINE label_line(dp, -1,-1,-1,  -1,-1,-1, FALSE)

/* The size of the full resolution space */
#define IMAGE_SPACE_WIDTH       2048
#define IMAGE_SPACE_HEIGHT      2048

/* The size of the individual pictures */
#define INDV_PIC_WIDTH  256
#define INDV_PIC_HEIGHT 256

/* How many individual pictures we are doing */
#define NUM_COLS    3
#define NUM_ROWS    3

/* The top and right margins of each individual picture.
   These center it in the space */
#define RIGHT_MARGIN    (((IMAGE_SPACE_WIDTH /NUM_COLS) - INDV_PIC_WIDTH )/2)
#define TOP_MARGIN      (((IMAGE_SPACE_HEIGHT/NUM_ROWS) - INDV_PIC_HEIGHT)/2)

#define HOW_OFTEN_TO_LABEL  1       /* Once a second */

/* local function prototypes */
static void mk_pixels
   (int color, int row, char *buf, int buflength) ;

int main( argc, argv)
    int argc ;
    char *argv[] ;
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;

    time_t start_time = time(NULL) ;

    unsigned char desired_film_num ;

    int arg ;
    int user_speced_film = FALSE ;

    int row, col ;  /* Index's which picture we are doing */
    int color ;     /* Index's which color we are doing */
    int y ;         /* Index's which line # we are sending */
    char *buf ;     /* Where we build pixel data to send */


/* Check the arguments */
    if ( argc > MAX_ARGS)
    {   PRINT_USAGE ;
        exit(1) ;
    }
    
    /* Scan the arguments */
    for ( arg = 1 ; arg < argc ; arg++)
    {
        /* Filmnumber */
        if ( strnicmp(argv[arg], "-f", 2) == 0)
        {       user_speced_film = TRUE ;
                desired_film_num = (unsigned char) atoi( argv[arg] + 2 ) ;
        }
        else
        {   /* Bad argument */
            printf ("Unknown argument: %s\n", argv[arg] ) ;
            PRINT_USAGE ;
            exit(1) ;
        }
    }
    
/* Tell 'um what is happening */
printf ( "An program built on top of the toolkit which shoots a bunch\n") ;
printf ( "of different pictures on the same piece of film.\n") ;
printf ( "Making %d x %d matrix of pictures, each row the same color.\n",
                NUM_COLS, NUM_ROWS) ;
printf ( "The pictures should vary in brightness from right to left\n") ;


/* Init the film printer */
    printf ("Initializing....\n") ;
    if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        print_err_and_exit( dp ) ;
    }
    
/* Change some default settings.
   We are going to place INDV_PIC_WIDTH x INDV_PIC_HEIGHT pictures in
   a IMAGE_SPACE_WIDTH x IMAGE_SPACE_HEIGHT resolution space on the film.
   Down in the inner loop below we will adjust the offsets to
   postition the film where we want */
    DP_HOR_RES(dp) = IMAGE_SPACE_WIDTH ;
    DP_VER_RES(dp) = IMAGE_SPACE_HEIGHT ;

    DP_LINE_LENGTH(dp) = INDV_PIC_WIDTH ;
    DP_VER_HEIGHT(dp)  = INDV_PIC_HEIGHT ;
    
    

    /* Set in user overrides */
    if ( user_speced_film )
    {   /* Set in the film type */
        printf ("Using film number %d\n", desired_film_num) ;
        DP_FILMNUMBER(dp) = desired_film_num ;
    }


    /* Get some memory to create an image in */
    if ( (buf = create_img_buf ( dp, 1)) == NULL )
    {   printf ("Error getting memory for image\n") ;
        exit(1) ;
    }
    
/* Start the exposure */
    printf ("Starting Exposure...\n") ;
    if ( DP_StartExposure(dp) )
    {   print_err_and_exit(dp) ;    /* Some kind of error */
    }

/* Loop over each little picture */
for ( row=0 ; row < NUM_ROWS ; row++)
{   for (col=0 ; col < NUM_COLS ; col++)
    {
        /* Do one of the little pictures. */
        printf ("Starting picture at (%d,%d)...\n", row, col) ;

        /* We need to pick the H and V offsets to move the
           picture around where we want */
        DP_HOR_OFFSET(dp) =
            col * (IMAGE_SPACE_HEIGHT/NUM_COLS) + TOP_MARGIN ;
        DP_VER_OFFSET(dp) =
            row * (IMAGE_SPACE_WIDTH /NUM_ROWS) + RIGHT_MARGIN;


        /* We alter the exposure setting depending on which column */
        DP_LIGHTEN_DARKEN(dp) = (char) ((col+1) * 2) ;

        /* Send the film parameters */
        printf ("Sending parameters....\n") ;
        if ( DP_SendPrinterParams (dp) )
        {   print_err_and_exit(dp) ;    /* Some kind of error */
        }
    
        /*  Loop over each color of the little picture */
        for ( color=0 ; color < NUM_USER_COLORS ; color++)
        {   
            /* Loop over each line */
            for (y=0 ; y < DP_VER_HEIGHT(dp) ; y++)
            {   
                /* Create some data to send */
                mk_pixels ( color, row, buf, DP_LINE_LENGTH(dp) ) ;

                /* Send it */
                if ( DP_SendImageData(dp, y, buf, DP_LINE_LENGTH(dp), color))
                {   printf ("\n") ;
                    print_err_and_exit(dp) ;    /* Error */
                }
                
                /* Label the line */
                periodic_label_line(HOW_OFTEN_TO_LABEL, dp,
                    -1, -1, -1,    col + row * NUM_COLS, color, y,
                    TRUE ) ;
            }
            ERASE_STATUS_LINE ;
       }
    }
}


/* All done.  We change to show buffer warnings so we can hang around
   and watch the exposure */
    DP_WAITFORBUFFER(dp) = SHOW_BUFFER_WARNING ;
    if ( DP_TerminateExposure (dp, END_OF_EXPOSURE) != EXPOSUREACTIVE)
    {   print_err_and_exit(dp) ;    /* Some kind of error */
    }

    /* Tell 'um what we are doing */
    ERASE_STATUS_LINE ;
    printf ("Terminate Exposure command sent...\n") ;
    printf ("All pictures are in the film printer....\n") ;


    /* Continually monitor status. We stay in this loop until
       the exposure is complete */
    while ( periodic_label_line(HOW_OFTEN_TO_LABEL, dp,
                    -1, -1, -1,    -1, -1, -1, TRUE ))
    ;
    ERASE_STATUS_LINE ;

    
    /* All went OK */
    printf ("Done!            \n") ;    
    printf ("Program ran for %0.1f seconds\n",
            difftime( time(NULL), start_time) ) ;
return(0);
}
    
/* Form Feed  */
/* mk_pixels

This "renders" data for the mosaic program.  It produces an almost pure
primary color for each row.

 5-oct-90  tc   Initial
     
*/

#define PRIMARY_VALUE   220     /* Primary Color Value */
#define OTHERS_VALUE     50     /* What other two colors are */

static void
mk_pixels ( color, row, buf, buflength )
    int color ;      /* Curr color exposing */
    int row ;        /* Which "little" picture row exposing */
    char *buf ;      /* Output, where the pixels built */
    int buflength ;  /* Size of the "buf" */
{
    /* Local declarations */
    char fill_color ;

    /* Pick what to fill with */
    fill_color = (char)
		 (((row % NUM_USER_COLORS) == color) ? PRIMARY_VALUE : OTHERS_VALUE) ;

    /* Fill the buffer */
    while ( buflength-- > 0 )
        *buf++ = fill_color ;
    
    /* All done */
    return ;
    
}
