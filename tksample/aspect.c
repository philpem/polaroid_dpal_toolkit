/*  DEC/CMS REPLACEMENT HISTORY, Element ASPECT.C */
/*  *7    20-JUN-1991 16:00:03 BIELEFELD "fix to msc6.0 warnings" */
/*  *6     3-MAY-1991 12:03:45 BROGNA "" */
/*  *5    18-JAN-1991 04:52:20 BROGNA "MSC 6.0x" */
/*  *4    16-JAN-1991 08:43:18 CAMPBELL_T "180 Cape work, faster in box, compile under MSC 5" */
/*  *3     8-OCT-1990 16:46:50 CAMPBELL_T "Support for Toolkit name changes" */
/*  *2     5-OCT-1990 15:10:30 CAMPBELL_T "Toolkit:: a) going to 1 struct, b) bug fixes" */
/*  *1     1-OCT-1990 21:25:23 CAMPBELL_T "TOOLKIT EXAMPLE PROGRAM" */
/*  DEC/CMS REPLACEMENT HISTORY, Element ASPECT.C */
/* aspect.c

Usage: aspect [-ffilmnumber]

An program built on top of the toolkit which shoots a single
picture with a circle in it.  It demonstrates how to adjust
resolution to perserve aspect ratio.

30-aug-90  tc   Initial
17-sep-90  tc   Ported to 2nd round
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 8-oct-90  tc   Went to long arithemetic for aspect ratio
 8-oct-90  tc   Support toolkit change
                 SUPPRESS_BUF_WARNING --> SUPPRESS_BUFFER_WARNING

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dpalette.h"

#include "demosup.h"

#define MAX_ARGS    2           /* Max legal value of argc */
#define PRINT_USAGE (printf ("Usage: aspect [-fFILMNUMBER]\n" ))


/* This chooses the background and foreground color */

#define BKGND_COLOR(c)  ( color_grounds[(c)][0] )
#define FGND_COLOR(c)   ( color_grounds[(c)][1] )

/* Red Circle, Cyan background */
unsigned char color_grounds[NUM_USER_COLORS][2] =
{               /* Background   Foreground */
/* Red */       {   40,           200       },
/* Green */     {  150,            30       },
/* Blue */      {  150,            40       }    
} ;


#define LINE_LABEL  10             /* How often to label line output */


#define IMAGE_WIDTH    1024         /* Size of image to expose */


int main( argc, argv)
    int argc ;
    char *argv[] ;
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;



    struct CIRCLE circle ;

    time_t start_time = time(NULL) ;

    unsigned char desired_film_num ;
    int term_stat ;

    int arg ;
    int user_speced_film = FALSE ;

    long int x_aspect_ratio, y_aspect_ratio ; /* Aspect ratio of the film.
                                              We convert this to long to avoid
                                              overflow problems in the aspect
                                              calculations */

    int color ;
    char *buf ;
    int y ;

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
printf ("Make a single full frame picture with a circle in it.\n") ;
printf ("Demonstrates how to adjust resolution to perserve aspect ratio.\n") ;


/* Init the film printer */
    printf ("Initializing....\n") ;
    if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        print_err_and_exit( dp) ;
    }
    
/* Change some default settings */
    /* Set in user overrides */
    if ( user_speced_film )
    {   /* Set in the film type */
        printf ("Using film number %d\n", desired_film_num) ;
        DP_FILMNUMBER(dp ) = desired_film_num ;
    }


    /* Make a smaller picture */
    DP_HOR_RES(dp) = DP_LINE_LENGTH(dp) = IMAGE_WIDTH ;

    /* Choose the vertical resolution to put square pixels on the film */
    x_aspect_ratio = DP_X_ASPECT(dp, DP_FILMNUMBER(dp)) ;
    y_aspect_ratio = DP_Y_ASPECT(dp, DP_FILMNUMBER(dp)) ;
    printf("x_aspect: %d, y_aspect: %d\n", x_aspect_ratio, y_aspect_ratio);
    DP_VER_RES(dp) = DP_VER_HEIGHT(dp) = (int)
      ( ( (long) DP_HOR_RES(dp) * y_aspect_ratio ) / x_aspect_ratio ) ;

    printf ("Shooting a %d x %d picture\n",
                    DP_HOR_RES(dp), DP_VER_RES(dp) ) ;
    

    /* Pick the size and location of circle to make. */
                                    /* Center it */
    circle.x_center = DP_HOR_RES(dp) / 2 ;
    circle.y_center = DP_VER_RES(dp) / 2 ;
                                    /* Make it fit in the picture */
    circle.radius = MIN( circle.x_center, circle.y_center ) ;
    circle.radius = (int) (( (float) circle.radius ) * (float) 0.80) ;

    /* Get some memory to create an image in */
    if ( (buf = create_img_buf ( dp, 1)) == NULL )
    {   printf ("Error getting memory for image\n") ;
        exit(1) ;
    }
    
    /* Send the film parameters */
    if ( DP_SendPrinterParams (dp) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }
    
    /* Start the exposure */
    printf ("Starting Exposure ...\n") ;
    if ( DP_StartExposure(dp) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }

    /* Loop over each color */
    for ( color=0 ; color < NUM_USER_COLORS ; color++)
    {   
        /* Loop over each line */
        for (y=0 ; y < DP_VER_HEIGHT(dp) ; y++)
        {   
            /* Label the output */
            if ( (y % LINE_LABEL) == 0 )
            {   /* Show 'um lines send and exposed. */
                label_line (dp,
                    -1, -1, -1,         /* No render label */
                     0, color, y,      /* Xmit label */
                    TRUE) ;             /* Show exposure */
            }

            /* Make up a line of the circle */
            mk_circle_pixels ( DP_LINE_LENGTH(dp), y,
                &circle, BKGND_COLOR(color), FGND_COLOR(color),
                buf ) ;
        
            /* Send it */
            if ( DP_SendImageData(dp,
                                y, buf, DP_LINE_LENGTH(dp), color))
            {   printf ("\n") ;
                print_err_and_exit( dp) ;    /* Error */
            }
        }

        /* Clean up status line, this erases it */
        label_line(dp,   -1,-1,-1,   -1,-1,-1,  FALSE) ;

    }

    /* Done with this picture.  We want to trap warnings so we
       can prompt the user to remove the film for manual camera backs */
    term_stat = DP_TerminateExposure (dp, END_OF_EXPOSURE) ;
    if (term_stat ==  EXPOSUREACTIVE)
    {   /* We need to wait for current exposure to complete
           Show them the rest of the exposure status */
           while ( label_line(dp, -1,-1,-1,   -1,-1,-1,  TRUE) )
           {   sleep(1) ;  /* Wait a second, so we don't chew up film printer
                              CPU by continually asking for status */
           }
           
           /* Tell the user to pull the film */
           printf ("Exposure is complete\n") ;
           printf ("Please remove the film and Hit the Enter Key >> ") ;
           while (getchar() != '\n') ;       /* Make 'um hit the Enter Key */
        
    }
    else if (term_stat)
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }
    
/* All went OK */

label_line(dp,   -1,-1,-1,  -1,-1,-1,  FALSE) ;   /* erase status */
printf ("Done!\n") ;    
printf ("Program ran for %0.1f seconds\n",
            difftime( time(NULL), start_time) ) ;
return(0);
}
    
