/*  DEC/CMS REPLACEMENT HISTORY, Element APIC.C */
/*  *4    20-JUN-1991 15:59:24 BIELEFELD "fix to msc6.0 warnings" */
/*  *3     3-MAY-1991 12:03:32 BROGNA "" */
/*  *2    18-JAN-1991 04:52:13 BROGNA "MSC 6.0x" */
/*  *1    11-OCT-1990 20:06:08 CAMPBELL_T "RENAMED, UST TO BE RANPIC0.C" */
/*  DEC/CMS REPLACEMENT HISTORY, Element APIC.C */
/* apic.c

Usage: apic [-ffilmnumber]

An program built on top of the toolkit which shoots a picture consisting of
random pixels.

30-aug-90  tc   Initial
17-sep-90  tc   Ported to 2nd round
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 8-oct-90  tc   Changed SUPRESS_BUF_WARNING --> SUPRESS_BUFFER_WARNING
11-oct-90  tc   Renamed ranpic0.c --> apic.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dpalette.h"

#include "demosup.h"

#define MAX_ARGS    2           /* Max legal value of argc */
#define PRINT_USAGE (printf ("Usage: apic [-fFILMNUMBER]\n") )



#define LINE_LABEL  10             /* How often to label line output */
char *color_str[] = { "Red", "Green", "Blue" } ;

#define IMAGE_WIDTH     256     /* Size of image to expose */
#define IMAGE_HEIGHT    200

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
                desired_film_num = (unsigned char) (atoi(argv[arg] + 2)) ;
        }
        else
        {   /* Bad argument */
            printf ("Unknown argument: %s\n", argv[arg] ) ;
            PRINT_USAGE ;
            exit(1) ;
        }
    }
    
/* Tell 'um what is happening */
    printf ("Making a full frame %d x %d picture of random pixels\n",
            IMAGE_WIDTH, IMAGE_HEIGHT) ;

/* Init the film printer */
    printf ("Initializing....\n") ;
    if ( DP_InitPrinter(dp, SUPPRESS_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        print_err_and_exit( dp ) ;
    }
    
/* Change some default settings */
    /* Make a smaller picture */
    DP_HOR_RES(dp) = DP_LINE_LENGTH(dp) = IMAGE_WIDTH ;
    DP_VER_RES(dp) = DP_VER_HEIGHT(dp)  = IMAGE_HEIGHT ;

    /* Don't convert to RLE */
    DP_IMAGE_COMPRESSION(dp) = NON_RLE ;

    /* Set in user overrides */
    if ( user_speced_film )
    {   /* Set in the film type */
        printf ("Using film number %d\n", desired_film_num) ;
        DP_FILMNUMBER(dp) = desired_film_num ;
    }


    /* Send the film parameters */
    printf ("Sending parameters....\n") ;
    if ( DP_SendPrinterParams (dp) )
    {   print_err_and_exit(dp) ;    /* Some kind of error */
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

            /* Make up a random image line */
            mk_random_pixels ( DP_LINE_LENGTH(dp), 0, 255, buf) ;
        
            /* Send it */
            if ( DP_SendImageData(dp, y, buf, DP_LINE_LENGTH(dp), color))
            {   printf ("\n") ;
                print_err_and_exit(dp) ;    /* Error */
            }
        }
    }
    /* Clean up status line */
    printf ("\r                                           \r") ;


/* All done */
    if ( DP_TerminateExposure (dp, END_OF_EXPOSURE))
    {   print_err_and_exit(dp) ;    /* Some kind of error */
    }
    
    
    /* All went OK */
    printf ("Done!            \n") ;    
    printf ("Program ran for %0.1f seconds\n",
            difftime( time(NULL), start_time) ) ;

return(0);
}
    
