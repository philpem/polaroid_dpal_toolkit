/*  DEC/CMS REPLACEMENT HISTORY, Element TKFILM.C */
/*  *7    20-JUN-1991 16:06:55 BIELEFELD "fix to msc6.0 warnings" */
/*  *6     3-MAY-1991 12:14:40 BROGNA "" */
/*  *5    20-FEB-1991 08:55:58 CAMPBELL_T "Made films run 0 to 19" */
/*  *4    18-JAN-1991 04:58:15 BROGNA "MSC 6.0x" */
/*  *3    16-JAN-1991 09:07:28 CAMPBELL_T "180 Cape work, faster in box, compile under MSC 5" */
/*  *2     5-OCT-1990 15:11:09 CAMPBELL_T "Toolkit:: a) going to 1 struct, b) bug fixes" */
/*  *1     1-OCT-1990 21:31:19 CAMPBELL_T "Toolkit evalution/demo program" */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKFILM.C */
/* tkfilm.c

Usage: tkfilm

Retrieves and prints all the film information

19-sep-90  tc   Initial
27-sep-90  tc   Changed to show buffer warnings, so will print
                 if there is an exposure already in the box
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 5-oct-90  tc   Reaction to Bug Fixes in the Toolkit, mostly removed local
                   macro definitions
                Made film slots run 1-20 in lieu of 0-19 to match ImagePrint
30-jan-91  tc   Made film slots run 0-19 to match arguments with utilities
*/

#include <stdio.h>
#include "dpalette.h"
#include "demosup.h"

static void print_film( DP_DATA *dp, int f) ;

int main()
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;

    int f ;

    printf ("tkfilm, Retrieves and prints all the film information.\n") ;

    /* Init it */
    if ( DP_InitPrinter(dp, SHOW_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        printf ("Got an error Initing the film printer!\n") ;
        printf ("%s\n", printer_err_msg(dp) ) ;
        printf ("We will still try to print the status\n") ;
    }
            
/* Start printing it */

    printf ("These films are appropriate for the attached %s camera:\n",
        DP_CAMERADESCRIPTION(dp) ) ;
    for ( f=0 ; f < MAX_NUM_FILMS ; f++ )
    {
        if ( DP_CAMERABACKFILM(dp,f) )
            print_film ( dp,f) ;
    }
    
    printf ("These are films available for other cameras: \n") ;
    for ( f=0 ; f < MAX_NUM_FILMS ; f++ )
    {
        if ( DP_FILM_PRESENT(dp,f) && !DP_CAMERABACKFILM(dp,f) )
            print_film ( dp,f) ;
    }
    
    printf ("These are empty slots available for downloading:\n") ;
    for ( f=0 ; f < MAX_NUM_FILMS ; f++ )
    {   if ( !DP_FILM_PRESENT(dp,f) )
            print_film (dp,f) ;
    }
        
return(0);    
}

/* Print_film

Prints a line of text which describes film # "f".

20-sep-90  tc   Initial
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 5-oct-90  tc   Made film slot label run 1-20 instead of 0-19
                Changed formating on Camera Adjust to handle negative nums
30-jan-91  tc   Made film slot label run 0-19
*/

#if (FILM_NAME_SIZE != 24)
    The filmname size is hardwired to 24 in printf below
    The FILM_NAME_SIZE changed, better fix the printf
#endif

static void
print_film( dp, f)
    DP_DATA *dp ;
    int f       ;            /* Which film to print */
{
    
/* Local declarations */
    float ca_x, ca_y, ca_z ;    /* Scaled Camera Adjust values */

/* Print the base information */
printf ( "  flm%2d   a/r %3.2f %s %s %-24s",

    f,                                 /* Film # */

                                        /* Aspect Ratio */
    (float) DP_X_ASPECT(dp,f) / (float) DP_Y_ASPECT(dp,f),


    DP_BLACKANDWHITEFILM(dp,f) ?       /* Black and White Test */
             "B&W"              :
             "   "              ,
    DP_FILMWASDOWNLOADED(dp,f) ?       /* Downloaded */
             "DnLd"             :
             "    "             ,
    DP_FILM_NAME(dp,f) + FILM_NAME_LEAD_CHARS

) ;

/* Only print Camera Adjust values if they are non-zero */
    ca_x = (float) DP_CAMERA_ADJUST_X(dp, f) / 10.0 ;
    ca_y = (float) DP_CAMERA_ADJUST_Y(dp, f) / 10.0 ;
    ca_z = (float) DP_CAMERA_ADJUST_Z(dp, f) / 10.0 ;


#define CA_PRINT_LIMIT  ((float) 0.01)
    if ( (ca_x >= CA_PRINT_LIMIT)  ||
         (ca_y >= CA_PRINT_LIMIT)  ||
         (ca_z >= CA_PRINT_LIMIT)  )
    {
        printf (" xyz %5.2f %5.2f %5.2f", ca_x, ca_y, ca_z) ;
            
    }
    
/* Close the line */
    printf ("\n") ;
    

return ;
}
