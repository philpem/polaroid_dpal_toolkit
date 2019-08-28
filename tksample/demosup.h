/*  DEC/CMS REPLACEMENT HISTORY, Element DEMOSUP.H */
/*  *6     3-MAY-1991 12:04:53 BROGNA "" */
/*  *5    18-JAN-1991 22:48:04 GRABEL "changes to run under LSC on the MAC" */
/*  *4    18-JAN-1991 04:53:10 BROGNA "MSC 6.0x" */
/*  *3     5-NOV-1990 17:37:15 GRABEL "added include of tkhost.h" */
/*  *2     5-OCT-1990 15:10:45 CAMPBELL_T "Toolkit:: a) going to 1 struct, b) bug fixes" */
/*  *1     1-OCT-1990 21:28:45 CAMPBELL_T "Include file for toolkit demo progs" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DEMOSUP.H */
/* demosup.h
 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
	 
Includes for demo support code.  This is NOT part of the toolkit.

30-aug-90  tc   Initial
19-sep-90  tc   Added TRUE and FALSE
21-sep-90  tc   Added CIRCLE
28-sep-90  tc   Added proto def for:
                    filmware_earlier_than
 4-oct-90  tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
 4-oct-90  vg   Added defines for the LSC compiler on the Macintosh
22-oct-90  vg   Added the time_t typedef for LSC on the Macintosh
29-oct-90  vg   Added include tkhost.h
18-jan-91  vg   made args to stricmp and strnicmp chars
*/

#include "tkhost.h"     /* host and compiler environment */

/*  *** For Macintosh users ****
    This file is included in all of the demo programs.  The programs are
    setup to send no more than 25 lines of info to stdout and will then
    terminate.  The following #defines are used for LSC so that a
    'console' window is opened up and the window is not closed until
    a key is hit.  If you are using a compiler on the Mac other than
    LSC, a similiar mechanism will be needed.
*/

#ifdef MACINTOSH_OS
#ifdef LSCV3            /* LightSpeed C Version 3 */

#ifndef DEMO_MAIN_MAIN  /* declared in lscmain.c  */
#define main _main
#define exit mac_exit
double  difftime() ;
#endif

/*  time_t is a typedef which defines the form of the value returned by
    the time() function.  This function is typically provided in the system
    library.  For LSC, this is an unsigned long value
*/
#ifndef _TIME_T_DEFINED
typedef unsigned long time_t ;   /* define a time value            */
#define _TIME_T_DEFINED          /* avoid multiple def's of time_t */
#endif

#endif
#endif

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#ifndef MIN
#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)    ( (a) > (b) ? (a) : (b) )
#endif

/* Describes a circle's location in the image */
struct CIRCLE
{
    int radius ;    /* In pixels */

    int x_center ;  /* IN pixels, (0,0) is upper left corner */
    int y_center ;  /* x increases to right, y increases down */

} ;



/* Function prototypes */
void print_err_and_exit (DP_DATA *dp) ;
char *printer_err_msg   (DP_DATA *dp) ;
char *create_img_buf    (DP_DATA *dp, unsigned int num_img_lines) ;

void mk_random_pixels ( int num_pixels, unsigned char low, unsigned char high,
                                                                  char *buf) ;
                                                              
void mk_circle_pixels ( int num_pixels, int line_num, struct CIRCLE *circle,
                        unsigned char bkgnd_color, unsigned char fgnd_color,
                        char *buf ) ;

int periodic_label_line (
    int period,             /* How often to print */
    DP_DATA *dp,
    int render_pic, int render_color, int render_line,
    int xmit_pic,   int xmit_color,   int xmit_line,
    int want_exposure_label ) ;

int label_line (
    DP_DATA *dp,
    int render_pic, int render_color, int render_line,
    int xmit_pic,   int xmit_color,   int xmit_line,
    int want_exposure_label ) ;

int firmware_earlier_than (
    DP_DATA *dp,
    int reqd_version) ;      /* Version number to test against */

int stricmp ( char *string1, char *string2 ) ;

int strnicmp ( char *string1, char *string2, int count ) ;

void sleep( int n ) ;
