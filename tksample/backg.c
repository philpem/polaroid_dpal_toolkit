/*  DEC/CMS REPLACEMENT HISTORY, Element BACKG.C */
/*  *5    20-JUN-1991 16:00:40 BIELEFELD "fix to msc6.0 warnings" */
/*  *4     3-MAY-1991 12:03:59 BROGNA "" */
/*  *3    18-JAN-1991 04:52:27 BROGNA "MSC 6.0x" */
/*  *2    16-JAN-1991 08:43:44 CAMPBELL_T "180 Cape work, faster in box, compile under MSC 5" */
/*  *1     8-OCT-1990 16:49:14 CAMPBELL_T "TOOLKIT APPLICATION, WITH SHADED BACKGROUNDS" */
/*  DEC/CMS REPLACEMENT HISTORY, Element BACKG.C */
/* backg.c

Usage: backg [-ffilmnumber]

An program built on top of the toolkit which shoots a single
picture with a sweep background.  It also demonstrates color mapping
for those applications that don't generate very many colors.

 8-oct-90    tc   Initial
 8-oct-90  tc   Support toolkit change
                 SUPPRESS_BUF_WARNING --> SUPPRESS_BUFFER_WARNING


*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dpalette.h"

#include "demosup.h"

#define MAX_ARGS    2           /* Max legal value of argc */
#define PRINT_USAGE (printf ("Usage: backg [-fFILMNUMBER]\n" ))

#define LABEL_PERIOD        1   /* Seconds between status update */

#define IMAGE_WIDTH    4096         /* Size of image to expose */

/* local function prototypes */
static void set_our_colors (DP_DATA *dp) ;
static void mk_frame_pixels (DP_DATA *dp, int y, char *buf) ;

int main( argc, argv)
    int argc ;
    char *argv[] ;
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;

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
printf ("An program built on top of the toolkit which shoots a single\n");
printf (
 "picture with a sweep background.  It also demonstrates color mapping\n");
printf ("for those applications that don't generate very many colors.\n");



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

    /* Choose the vertical resolution to put square pixels on the film.
       Note that we do long arithemetic to avoid overflow */
    x_aspect_ratio = DP_X_ASPECT(dp, DP_FILMNUMBER(dp)) ;
    y_aspect_ratio = DP_Y_ASPECT(dp, DP_FILMNUMBER(dp)) ;
    if(x_aspect_ratio != 0)
    {
      DP_VER_RES(dp) = DP_VER_HEIGHT(dp) = (int)
	( ( (long) DP_HOR_RES(dp) * y_aspect_ratio ) / x_aspect_ratio ) ;
    }
    else
    {
      DP_VER_RES(dp) = DP_VER_HEIGHT(dp) = (int) ( ((float) DP_HOR_RES(dp))*3.25/4.25);
    }

    printf ("Shooting a %d x %d picture\n",
                    DP_HOR_RES(dp), DP_VER_RES(dp) ) ;
    

    /* Turn on background sweeps.
       We choose a vertical sweep in blue.  So we set the two upper
       corners the same and the two lower corners the same.  We
       pick 0 as the background color to replace */
    DP_BACKGROUND_MODE(dp) = BACKGROUND_LINEAR ;
    DP_BACKGROUND_PIX_VALUE(dp) = 0 ;

    DP_UPPER_LEFT_COLOR (dp, RED) = DP_UPPER_RIGHT_COLOR(dp, RED) = 50 ;
    DP_LOWER_LEFT_COLOR (dp, RED) = DP_LOWER_RIGHT_COLOR(dp, RED) = 75 ;

    DP_UPPER_LEFT_COLOR (dp, GREEN) = DP_UPPER_RIGHT_COLOR(dp, GREEN) = 50 ;
    DP_LOWER_LEFT_COLOR (dp, GREEN) = DP_LOWER_RIGHT_COLOR(dp, GREEN) = 75 ;

    DP_UPPER_LEFT_COLOR (dp, BLUE) = DP_UPPER_RIGHT_COLOR(dp, BLUE) = 50 ;
    DP_LOWER_LEFT_COLOR (dp, BLUE) = DP_LOWER_RIGHT_COLOR(dp, BLUE) = 255 ;


    /* We only generate an image with 8 colors in it.  The pixels values
       we create take on values 0 to 7.  So let us make those pixel
       values to interesting colors: */
    set_our_colors(dp) ;

    
    

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
            /* Label the output,
               Show 'um lines send and exposed. */
               periodic_label_line (LABEL_PERIOD, dp,
                    -1, -1, -1,         /* No render label */
                     0, color, y,      /* Xmit label */
                    TRUE) ;             /* Show exposure */

            /* Make up a line of the image we want */
            mk_frame_pixels ( dp, y,  buf ) ;
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
           while ( periodic_label_line(LABEL_PERIOD, dp,
                                           -1,-1,-1,   -1,-1,-1,  TRUE) )
           ;
           
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
    
/* Form Feed  */
/* set_our_colors

This changes the color table in "dp" to only specify 8 colors of our
choosing.

 8-oct-90  tc   Initial
     
*/

static void
set_our_colors (dp)
    DP_DATA *dp ;   /* Where the color table lives */
{
    /* Local declarations */
    int color ; /* For indexing color table */
    int entry ; /*    ditto */

    /* We only generate an image with 8 colors in it.  The pixels values
       we create take on values 0 to 7.  So let us make those pixel
       values to interesting colors: */

#define OUR_BLACK   0
#define OUR_RED     1
#define OUR_GREEN   2
#define OUR_BLUE    3
#define OUR_YELLOW  4   /* Red + Green */
#define OUR_MAGENTA 5   /* Red + Blue */
#define OUR_CYAN    6   /* Green + Blue */
#define OUR_WHITE   7   /* Red + Green + Blue */


    /* Start by initing all of the color values to 255.
       This is the value that should be set in for unused colors */
    for (color=0 ; color < NUM_USER_COLORS ; color++)
    {   for (entry=0 ; entry < COLOR_TABLE_SIZE ; entry++)
        {   DP_COLOR_TABLE(dp, color, entry) = 255 ;
        }
    }

    /* Now set our values in */
    DP_RED_COLOR_TABLE  (dp, OUR_BLACK) = 0 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_BLACK) = 0 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_BLACK) = 0 ;

    DP_RED_COLOR_TABLE  (dp, OUR_RED) = 255 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_RED) =   0 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_RED) =   0 ;

    DP_RED_COLOR_TABLE  (dp, OUR_GREEN) =   0 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_GREEN) = 255 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_GREEN) =   0 ;

    DP_RED_COLOR_TABLE  (dp, OUR_BLUE) =    0 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_BLUE) =    0 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_BLUE) =  255 ;

    DP_RED_COLOR_TABLE  (dp, OUR_YELLOW) = 255 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_YELLOW) = 255 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_YELLOW) =   0 ;

    DP_RED_COLOR_TABLE  (dp, OUR_MAGENTA) = 255 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_MAGENTA) =   0 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_MAGENTA) = 255 ;

    DP_RED_COLOR_TABLE  (dp, OUR_CYAN) =    0 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_CYAN) =  255 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_CYAN) =  255 ;

    DP_RED_COLOR_TABLE  (dp, OUR_WHITE) = 255 ;
    DP_GREEN_COLOR_TABLE(dp, OUR_WHITE) = 255 ;
    DP_BLUE_COLOR_TABLE (dp, OUR_WHITE) = 255 ;
        
   /* That's all */
   return ;
}
    
/* Form Feed  */
/* mk_frame_pixels

This function a line "y" of an image in "buf".

The image is a series of frames of different colors with black between
each frame.  It is probably easier to run the program to see the output,
but I will try to describe it:

        RRRRRRRR
        R      R
        R BBBB R
        R B  B R
        R B  B R
        R BBBB R
        R      R
        RRRRRRRR

Notice we don't care what color we are rendering because we are color mapped.
We generate the same pixel value for each line regardless of color.  The
definitions of OUR_color are from the set_our_colors() function.

 8-oct-90  tc   Initial
*/
static void
mk_frame_pixels (dp, y, buf)
    DP_DATA *dp ;   /* Describes the image space */
    int y ;         /* Which line */
    char *buf ;     /* Where to make the pixels */
{
    /* Our basic data type */
    struct A_FRAME
    {   char color ;    /* What color the frame is */

        int lft_x ;     /* (x,y) of upper left corner */
        int top_y ;
        
        int rht_x ;     /* (x,y) of lower right corner */
        int bot_y ;
    } ;

/* Here is where we define the frames.  The are defined assuming that they
   start at (0,0).  The first time thru this function, we adjust their
   location to center them on the film */
#define NUM_FRAMES  9
    static int first_time = TRUE ;
    static struct A_FRAME frame[] =
    {
        { OUR_RED,      -500, -500,      500,  500} ,
        { OUR_BLACK,    -450, -450,      450,  450} ,
        { OUR_GREEN,    -400, -400,      400,  400} ,
        { OUR_BLACK,    -350, -350,      350,  350} ,
        { OUR_YELLOW,   -300, -300,      300,  300} ,
        { OUR_BLACK,    -250, -250,      250,  250} ,
        { OUR_CYAN,     -200, -200,      200,  200} ,
        { OUR_BLACK,    -150, -150,      150,  150} ,
        { OUR_WHITE,    -100, -100,      100,  100}
    } ;

    /* Local declarations */
    int x ;                 /* For indexing */
    char *ptr, *end_ptr ;   /* For setting */
    int curr_frame ;        /* For indexing frame table */
    char color ;            /* Current frame color */

    /* First time only, adjust frame location to center on film. We don't
       do this staticly because the vertical resolution changes depending
       on the film */
    if ( first_time )
    {   for ( curr_frame=0 ; curr_frame < NUM_FRAMES ; curr_frame++)
        {   frame[curr_frame].lft_x  +=  (DP_HOR_RES(dp)/2) ;
            frame[curr_frame].top_y  +=  (DP_VER_RES(dp)/2) ;
            frame[curr_frame].rht_x  +=  (DP_HOR_RES(dp)/2) ;
            frame[curr_frame].bot_y  +=  (DP_VER_RES(dp)/2) ;
        }
    }
    first_time = FALSE ;

    /* Set the line to all black */
    for ( x=0, ptr=buf ; x < DP_LINE_LENGTH(dp) ; x++)
        *ptr++ = OUR_BLACK ;
    
    /* Generate each frame in turn.
       If you hadn't noticed, This isn't the most efficient algorithm */
    for (curr_frame=0 ; curr_frame < NUM_FRAMES ; curr_frame++)
    {   /* If current line is within the frame, fill in the square */
        if ( (y >= frame[curr_frame].top_y) &&
             (y <= frame[curr_frame].bot_y) )
        {   /* Fill with current color */
            color = frame[curr_frame].color ;
            ptr     = buf + frame[curr_frame].lft_x ;
            end_ptr = buf + frame[curr_frame].rht_x ;
            while ( ptr <  end_ptr )
            {   *ptr++ = color ;
            }
        }
    }
    
    return ;
    
}
