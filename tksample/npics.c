/*  DEC/CMS REPLACEMENT HISTORY, Element NPICS.C */
/*  *5    20-JUN-1991 16:05:20 BIELEFELD "fix to msc6.0 warnings" */
/*  *4     3-MAY-1991 12:08:03 BROGNA "" */
/*  *3    18-JAN-1991 04:55:45 BROGNA "MSC 6.0x" */
/*  *2    16-JAN-1991 08:59:42 CAMPBELL_T "180 Cape work, faster in box, compile under MSC 5" */
/*  *1    11-OCT-1990 20:06:44 CAMPBELL_T "renamed, used to be nranpics.c" */
/*  DEC/CMS REPLACEMENT HISTORY, Element NPICS.C */
/* npics.c

Usage: npics [-ffilmnumber]

An program built on top of the toolkit which shoots multiple
pictures consisting of random pixels, with a bias to produce
red, green, and blue images in sequence

30-aug-90  tc   Initial
17-sep-90  tc   Ported to 2nd round
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 5-oct-90  tc   Removed patches to Terminate Exposure.  The Toolkit itself
                was fixed.
11-oct-90  tc   Renamed nranpics.c --> npics.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dpalette.h"

#include "demosup.h"

#define MAX_ARGS    3           /* Max legal value of argc */
#define PRINT_USAGE (printf ("Usage: npics [-fFILMNUMBER] [-nNUMPICS]\n" ))




#define SECS_BETWEEN_LABEL      1   /* How often to label line output */


#define IMAGE_WIDTH     256     /* Size of image to expose */
#define IMAGE_HEIGHT    200     /* Make 'um small so demo is fast */

#define DEF_NUM_PICS    3


/* Use a table to choose the range of random pixels to generate.
   This produces the red, green, and blue bias.
   These macros produce the lower and upper limits of for the random
   pixel generator.
       int  p   picture number
       int  c   which color
*/
#define LOWER_BND(p,c) ((unsigned char) (bound_table[ ((p)%3) ] [ (c) ] [0] ))
#define UPPER_BND(p,c) ((unsigned char) (bound_table[ ((p)%3) ] [ (c) ] [1] ))

int bound_table[3][3][2] =
{              /* Red  Green  Blue */
/* pic 0 */    {  {128, 255}, {  0, 100}, {  0, 100}  },
/* pic 1 */    {  {  0, 100}, {128, 255}, {  0, 100}  },
/* pic 2 */    {  {  0, 100}, {  0, 100}, {128, 255}  }
} ;

int main(argc, argv)
    int argc ;
    char *argv[] ;
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;

    time_t start_time = time(NULL) ;

    unsigned char desired_film_num ;
    int number_pictures_to_take = DEF_NUM_PICS ;
    int term_stat, send_stat ;
    

    int arg ;
    int user_speced_film = FALSE ;

    int pic ;
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
        else if ( strnicmp(argv[arg], "-n", 2) == 0)
        {       number_pictures_to_take = atoi( argv[arg] + 2 ) ;
        }
        else
        {   /* Bad argument */
            printf ("Unknown argument: %s\n", argv[arg] ) ;
            PRINT_USAGE ;
            exit(1) ;
        }
    }
    
/* Tell 'um what is happening */
PRINT_USAGE ;
printf ("Making %d full frame %d x %d pictures of random pixels\n",
            number_pictures_to_take, IMAGE_WIDTH, IMAGE_HEIGHT) ;
printf ("The pictures will come out REDish, GREENish, and BLUEish\n");

/* Init the film printer.
   Note, the SHOW_BUFFER_WARNING tells the toolkit that we want to
         do things (like update status) if you can't send data to the film
         printer because its buffer is full */
printf ("Initializing....\n") ;
if ( DP_InitPrinter(dp, SHOW_BUFFER_WARNING, FALSE) )
{   /* Error initing */
    print_err_and_exit( dp) ;
}
    
/* Change some default settings */
/* Make a smaller picture */
DP_HOR_RES(dp) = DP_LINE_LENGTH(dp) = IMAGE_WIDTH ;
DP_VER_RES(dp) = DP_VER_HEIGHT(dp)  = IMAGE_HEIGHT ;


/* Set in user overrides */
if ( user_speced_film )
{   /* Set in the film type */
    printf ("Using film number %d\n", desired_film_num) ;
        DP_FILMNUMBER(dp ) = desired_film_num ;
}


/* Get some memory to create an image in */
if ( (buf = create_img_buf ( dp, 1)) == NULL )
{   printf ("Error getting memory for image\n") ;
    exit(1) ;
}
    
/* Loop over each picture */
for ( pic=0 ; pic < number_pictures_to_take ; pic++)
{
    /* Send the film parameters */
    if ( DP_SendPrinterParams (dp) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }
    
    /* Start the exposure */
    printf ("Starting Exposure #%d...\n", pic) ;
    if ( DP_StartExposure(dp) )
    {   print_err_and_exit( dp) ;    /* Some kind of error */
    }

    /* Loop over each color */
    for ( color=0 ; color < NUM_USER_COLORS ; color++)
    {   
        /* Loop over each line */
        for (y=0 ; y < DP_VER_HEIGHT(dp) ; y++ )
        {   

            /* Make up a random image line */
            mk_random_pixels ( DP_LINE_LENGTH(dp),
                LOWER_BND(pic,color), UPPER_BND(pic,color), buf) ;
        
            /* Update status and try to send the line */
            do
            {   
                /* Label the output periodically.
                   Show 'um lines send and exposed. */
                periodic_label_line ( SECS_BETWEEN_LABEL,
                        dp,
                        -1, -1, -1,         /* No render label */
                        pic, color, y,      /* Xmit label */
                        TRUE) ;             /* Show exposure */

                
                /* Try to send the line of data to film printer */
                send_stat = DP_SendImageData(dp,
                        y, buf, DP_LINE_LENGTH(dp), color) ;
                    
               /* Check for errors.  It had better be OK or
                  didn't send the line because buffer is full */
               if ( (send_stat != NOERROR) && (send_stat != BUFFERWARNING))
               {    /* Got some kind of real error */
                   printf ("\n") ;
                   print_err_and_exit( dp) ;    /* Error */
               }

           }while (send_stat != NOERROR) ;   /* Stay in loop til line sent */
       } /* End of a color, sent all lines */

        /* Clean up status line, this erases it */
        label_line(dp,   -1,-1,-1,   -1,-1,-1,  FALSE) ;

    } /* End of a picture, sent all the colors */

    /* Done with this picture.  We want to trap warnings so we
       can prompt the user to remove the film for manual camera backs */
    term_stat = DP_TerminateExposure (dp, END_OF_EXPOSURE) ;
    if (term_stat ==  EXPOSUREACTIVE)
    {   /* We need to wait for current exposure to complete
           Show them the rest of the exposure status */
           while ( periodic_label_line( SECS_BETWEEN_LABEL,
                       dp, -1,-1,-1,   -1,-1,-1,  TRUE) )
           ;
           
           /* Tell the user to pull the film */
           printf ("Exposure %d is complete\n", pic) ;
           printf ("Please remove the film and Hit the Enter Key >> ") ;
           while (getchar() != '\n') ;       /* Make 'um hit the Enter Key */
        
    }
    else if (term_stat)
    {   /* Anything else is a legitimate error */
        print_err_and_exit( dp) ;    /* Some kind of error */
    }
    
}
    
/*  Pictures all done, hang around showing status until they complete */
while ( periodic_label_line( SECS_BETWEEN_LABEL,
                             dp, -1,-1,-1,   -1,-1,-1,  TRUE) )
;

/* All went OK */

label_line(dp,   -1,-1,-1,  -1,-1,-1,  FALSE) ;   /* erase status */
printf ("Done!\n") ;    
printf ("Program ran for %0.1f seconds\n",
            difftime( time(NULL), start_time) ) ;
return(0);
}
    
