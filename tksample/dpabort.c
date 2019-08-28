/*  DEC/CMS REPLACEMENT HISTORY, Element DPABORT.C */
/*  *3    20-JUN-1991 16:02:41 BIELEFELD "fix to msc6.0 warnings" */
/*  *2     3-MAY-1991 12:05:02 BROGNA "" */
/*  *1    18-JAN-1991 17:24:10 CAMPBELL_T "DEMO PROGRAM WHICH ABORTS AN EXPOSURE" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPABORT.C */
/* dpabort.c

    Small utility program which will abort the exposure of an image in
    the filmprinter.  This little tool comes in handy during software
    development.
        
    We use the ABORT_EXPOSURE argument in the DP_TerminateExposure
    function in the Digital Palette Toolkit.

14-jan-91  vg       initial....
*/

#include <stdio.h>
#include "dpalette.h"
#include "demosup.h"

int main(argc, argv)
int argc ;
char *argv[] ;
{
    /* Local declarations */
    DP_DATA our_dp ;
    DP_DATA *dp = &our_dp ;
    int status ;

    /* Init the toolkit and printer */
    status = DP_InitPrinter(dp, SHOW_BUFFER_WARNING, FALSE) ;
    
    if ( status && ( status != EXPOSUREACTIVE ))
    {   printf ("Got an error we shouldn't have!!!\n") ;
        print_err_and_exit( dp) ;
    }

    if ( DP_TerminateExposure(dp, ABORT_EXPOSURE) != NOERROR )
        print_err_and_exit( dp) ;

    return(0) ;
}

