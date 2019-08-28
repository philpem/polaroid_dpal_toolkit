/*  DEC/CMS REPLACEMENT HISTORY, Element DEMOSUP.C */
/*  *9    20-JUN-1991 16:01:59 BIELEFELD "fix to msc6.0 warnings" */
/*  *8     3-MAY-1991 12:04:42 BROGNA "" */
/*  *7    18-JAN-1991 22:47:49 GRABEL "changes to run under LSC on the MAC" */
/*  *6    18-JAN-1991 04:53:01 BROGNA "MSC 6.0x" */
/*  *5     5-NOV-1990 17:37:09 GRABEL "added string functions" */
/*  *4    16-OCT-1990 16:31:40 BROGNA "ABORT/Remove Film Bug fix" */
/*  *3     8-OCT-1990 16:47:00 CAMPBELL_T "Support for Toolkit name changes" */
/*  *2     5-OCT-1990 15:10:39 CAMPBELL_T "Toolkit:: a) going to 1 struct, b) bug fixes" */
/*  *1     1-OCT-1990 21:28:13 CAMPBELL_T "SUPPORT CODE FOR TOOLKIT EVALUATION PROGRAMS" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DEMOSUP.C */
/* demosup.c

Support programs for tc's demo programs on top of the toolkit.

30-aug-90  tc   Initial
21-sep-90  tc   Added circle generater
27-sep-90  tc   Added periodic_label_line()
28-sep-90  tc   Moved sleep() to another file
                Added firmware_earlier_than()
4-oct-90   tc   Changed to support POLAPARAMS, POLASTATUS --> DP_DATA
                De-ANSI C'ed the function declarations so will port to
                    MAC, SUN
 5-oct-90  tc   Bug fix, was misusing PRINTERERROR()
 8-oct-90  tc   label_line(): Shortened up the line a bit
16-oct-90  mb   Added NOERROR support to printer_err_msg()
22-oct-90  vg   added a stricmp function ( used in demo apps and not ANSI )
                added a strnicmp function     ...ditto...
18-jan-91  vg   made args to stricmp and strnicmp chars

Table of contents:
    printer_err_msg     Returns an error message string
    print_err_and_exit  Prints curr error msg on stdout and exits

    create_img_buf      Allocates memory for lines of image data

    mk_random_pixels    Generates a line of random pixels
    mk_circle_pixels    Generates a line for a circle

    periodic_label_line Every so often calls label_line()
    label_line          Prints to stdout # of lines exposed
        
    firmware_earlier_than   Checks revision of firmware
    
*/

#include <stdio.h>

#include <ctype.h>
#include <time.h>
#include <math.h>  /* For mk_circle_pixels() */

#include "dpalette.h"
#include "demosup.h"

/* Prototypes */
void strcpy (char *, char *) ;
void strcat (char *, char *) ;
int  strlen (char *) ;

/* The following include file is needed to define the malloc() function */
#include <stdlib.h>

/* Form Feed  */
/* printer_err_msg

This function returns an error message describing the error condition of
the Digital Palette.

Their is NO \n on the end of the string.

Function returns NULL if there is no error.

30-aug-90  tc   Initial
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
                Updated to handle all the error returns
                Shortened the error msg header that I generate.
 5-oct-90  tc   Bug fix, was misusing PRINTERERROR()
*/

char *printer_err_msg ( dp )
    DP_DATA *dp ;
{
    /* Local declarations */
    static char    err_msg[1024] ;  /* What we return */
    char           working_msg[512] ; /* Part of err_msg built here */


   /* Determine nature of error */
   if ( DP_ERROR(dp) )
   {    strcpy(err_msg, "ERR: ") ;
   }
   else if ( DP_WARNING(dp) )
   {    strcpy(err_msg, "WARNING: ") ;
   }
   else
   {    /* No error */
        strcpy(err_msg, "OKAY: ") ;
   }

   /* Label the class of error,
      We init err_msg here */       
   switch(DP_ERRORCLASS(dp))
   {
         case LOGICERR:
            strcat(err_msg, "LE") ;
            break ;
         case HARDWERR:
            strcat(err_msg, "HE") ;
            break ;
         case TIMEOUT:
            strcat(err_msg, "TO") ;
            break ;
         case DISCONNECTED:
            strcat(err_msg, "DC") ;
            break ;
         case UNKNOWNPORT:
            strcat(err_msg, "UP") ;
            break ;
         case CONFIGERR:
            strcat(err_msg, "CE") ;
            break ;
        case DPTOOLKITERR:
            strcat(err_msg, "TK") ;
            break ;
        case BUFFERWARNING:
            strcat(err_msg, "BF") ;
            break ;
        case EXPOSUREACTIVE:
            strcat(err_msg, "EA") ;
            break ;
        case REMOVEFILM:
            strcat(err_msg, "RF") ;
            break ;
        case NOERROR:
            break ;

         default:
           sprintf(err_msg, "Unknown Error Condition, Class=%d",
               DP_ERRORCLASS(dp) ) ;
           break ;
         }


      /* If applicable, give um the error number.
          We build this message in working_msg, and tack it on */
      if (DP_ERRORNUMBER(dp))
      {   sprintf (working_msg, ", #%d ", DP_ERRORNUMBER(dp)) ;
          strcat (err_msg, working_msg) ;
      }

      /* and give 'um a full error message */
      if( DP_ERRORMESSAGE(dp)[0] )
      {   strcat (err_msg, ": ") ;
          strcat (err_msg, DP_ERRORMESSAGE(dp)) ;
      }

      /* All done */
      return err_msg ;
    
}


/* Form Feed  */
/* print_err_and_exit

Gets the error message from "status", prints it and exits.
THIS IS NOT PART OF THE TOOLKIT.

30-aug-90  tc   Initial
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
                Added print of initial \n in case in middle of status
*/
    
void print_err_and_exit ( dp )
    DP_DATA *dp ;
{
    /* Local declarations */
    char *err_msg ;

    err_msg = printer_err_msg (dp) ;
    printf ("\n%s\n", err_msg) ;
    exit(1) ;
}

/* Form feed  */
/* create_img_buf

This function allocates memory for "num_img_lines" of image data.  The
width of each line is taken from the "linelength" parameter of "dp".

It normally returns a pointer to the memory, but returns NULL to indicate
an error.

30-aug-90  tc   Initial
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
*/

char *create_img_buf ( dp, num_img_lines)
    DP_DATA *dp ;
    unsigned int num_img_lines ;
{
    return
        malloc ( num_img_lines * DP_LINE_LENGTH(dp ) ) ;
}

/* Form Feed  */
/* mk_random_pixels

This function sets "num_pixels" starting at "buf" to random values between
"low" and "high"

30-aug-90  tc   Initial
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
*/
void mk_random_pixels ( num_pixels, low, high, buf)
    int num_pixels;     /* How many to do */
    unsigned char low;  /* Lowest pixel value used */
    unsigned char high; /* Highest pixel value used */
    char *buf;          /* Where the pixels live */
{
    while ( num_pixels-- > 0 )
    {   *(buf++) = low + (char) (rand() % (int) (high - low + 1)) ;
    }
    
    return ;

}

/* Form Feed  */
/* mk_circle_pixels

This generates a line of pixel data at "buf", which is assumed to be
"num_pixels" long.

"y" is the line number in the image being generated.

"circle" describes the location.  "bkgnd_color" is used for the area
outside the circle and "fgnd_color" is used for inside the circle.

21-sep-90  tc   Initial
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
*/
void mk_circle_pixels ( num_pixels, line_num,
                                circle, bkgnd_color, fgnd_color, buf )
    int num_pixels;                 /* Size of "buf" */
    int line_num;                   /* Y coordinate of "buf" */
    struct CIRCLE *circle;          /* Describes circle location */
    unsigned char bkgnd_color;      /* Outside color */
    unsigned char fgnd_color;       /* Inside color */
    char *buf ;                     /* Where pixels generated */
{
    /* Local declarations */
    float radius, Xcenter, Ycenter, Y ; /* Converted args */
    float term ;                        /* working temp */
        
    float Xright, Xleft ;               /* Where we compute intersection pt */

    int num_left_background_pixels ;    /* What we compute on how */
    int num_foreground_pixels ;         /*    to generate the line */
    int num_right_background_pixels ;


    /* We need to find where the circle intersects this horizontal line.
       It can be 0, 1, or 2 places.
       Solve the circle equation for x:
           (X - Xcenter)^2 + (Y - Ycenter)^2 = Radius^2
            X = SQRT [ Radius^2 - (Y - Ycenter)^2] + Xcenter
       Note: since we are using a 4th quadrant of the cord. system,
           we invert y.
       We produce three variables:
           num_left_background_pixels
           num_foreground_pixels
           num_right_background_pixels
       These will all sum to num_pixels
    */
    
    /* Convert everything to float to make code read easier */
    radius  = (float)   circle->radius ;
    Xcenter = (float)   circle->x_center ;
    Ycenter = (float) - circle->y_center ;          /* 4th Quadrant */
    Y       = (float) - line_num ;                 /* 4th Quadrant */
    
    /* Compute what goes inside the SQRT */
    term = radius * radius -
           (Y - Ycenter) * (Y - Ycenter) ;
       
   /* Check for no intersection */
   if ( term < (float) 0 )
   {   /* Circle not on this line, all background */
       num_left_background_pixels  = num_pixels ;
       num_foreground_pixels       =
       num_right_background_pixels = 0 ;
   }
   else
   {    /* We have an intersection, compute where it is.
           We take the negative SQRT result to produce the left crossing
           We take the right SQRT result to produce the right crossing */
        if ( (Xleft = (float) (-sqrt(term)) + Xcenter) < (float) 0 )
        {   /* Clip */
            Xleft = (float) 0 ;
        }
        if ( (Xright  = (float) (sqrt(term)) + Xcenter) >= (float) num_pixels)
        {   /* Clip */
            Xright = (float) num_pixels  ;
        }
        
        num_left_background_pixels  =              (int) Xleft ;
        num_right_background_pixels = num_pixels - (int) Xright ;
        num_foreground_pixels = num_pixels -
            (num_left_background_pixels + num_right_background_pixels ) ;
    }
    
    /* Generate the pixels */
    while ( num_left_background_pixels-- > 0 )
        *buf++ = bkgnd_color ;

    while ( num_foreground_pixels-- > 0 )
        *buf++ = fgnd_color ;

    while ( num_right_background_pixels-- > 0 )
        *buf++ = bkgnd_color ;

    
    /* All done */
    return ;
        
}

/* Form Feed  */

/* periodic_label_line

Calls label_line() if it has been "period" seconds since the last
call to label_line().  All of the arguments past "period" are passed
along to label_line().

See label_line() for a description.

This function is used to limit how often the film printer is inquired
(to save film printer CPU cycles) and how often the screen is painted.

The function returns what label_line() returns:
    TRUE to indicate exposure in process and FALSE otherwise.
In between calls to label_line() it returns want_expsosure label(), i.e.
    it typically says an exposure is in process.

27-sep-90  tc   Initial
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
*/

/* Local declarations */
static time_t time_last_called_label_line = 0  ;

int periodic_label_line (period, dp,  render_pic, render_color, render_line,
                                      xmit_pic, xmit_color, xmit_line,
                                      want_exposure_label )
    int period;             /* How often to run it */
    DP_DATA *dp ;           /* Handle on film printer */
    int render_pic; int render_color; int render_line;  /* How far rendered */
    int xmit_pic;   int xmit_color;   int xmit_line;    /* How far sent */
    int want_exposure_label ;                   /* T--> want exposure label */
{
           time_t curr_time ;

    /* See if time to call 'um */
    curr_time = time(NULL) ;
    if ( difftime( curr_time, time_last_called_label_line) >= (double) period)
    {   /* Time to do it again */
        time_last_called_label_line = curr_time ;
        return label_line( dp,
            render_pic, render_color, render_line,  /* How far rendered */
            xmit_pic,   xmit_color,   xmit_line,    /* How far sent */
            want_exposure_label )                   /* T--> want exp label */
        ;
    }
    else
    {   /* Ain't time yet */
        return want_exposure_label ;
    }
}

/* Form Feed  */

/* label_line

Prints to stdout a message showing picture #, color, and
the number of lines:
    rendered
    xmitted
    exposed
        
"render_pic, render_color, render_line" describe how much the application
has rendered (when you are pipeling the rendering and transmission).  Set
to -1 to disable output.

"xmit_pic, xmit_color, xmit_line" describe how much you have sent to the
film printer.  Set to -1 to disable output.

"want_exposure_label" controls whether this routine goes out to the film
printer to see how much it has exposed.

The labeling overwrites a single line on stdout.  Set all outputs off
(-1's and FALSE) to complete erase the status line.

Function returns TRUE if the film printer is exposing.  If you tell
me to not check the box via want_exposure_label == FALSE, I return FALSE)

Example:
  Rndring P#4,GRN-1234  Xmiting P#4,RED-900  Exposing P#4,RED-600 Buf: 67%

21-sep-90  tc   Initial
27-sep-90  tc   Added Buffer % full
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
 8-oct-90  tc   Shortened up the line a bit
*/
int label_line (dp,  render_pic, render_color, render_line,
                     xmit_pic,   xmit_color,   xmit_line,
                                                     want_exposure_label )
    DP_DATA *dp ;           /* Handle on film printer */
    int render_pic; int render_color; int render_line;  /* How far rendered */
    int xmit_pic;   int xmit_color;   int xmit_line;    /* How far sent */
    int want_exposure_label ;                   /* T--> want exposure label */

 {
    /* Local declarations */
    static char *color_str[] = { "RED", "GRN", "BLU" } ;
    static int last_xmit_pic = 0 ;  /* Where we remember the last
                                    picture number xmitted.  This is used
                                    to produce the picture number for
                                    the exposing entry */
    
    int exp_pic, exp_color, exp_line ;
    float percent_full ;            /* How full the buffer is */
    int exposing = FALSE ;  /* What we return */


    /* Beancount on the picture number being xmitted */
    if ( xmit_pic != -1 )
    {   last_xmit_pic = xmit_pic ;
    }

    /* Rendering */
    if ( render_pic != -1 )
    {   printf ("  Rndring P#%d,%s-%4d",
                        render_pic, color_str[render_color], render_line) ;
    }
    else
    {   printf ("                      " ) ;
    }
    
    /* Xmitting */
    if ( xmit_pic != -1 )
    {   printf ("  Xmiting P#%d,%s-%4d",
                        xmit_pic, color_str[xmit_color], xmit_line) ;
    }
    else
    {   printf ("                      " ) ;
    }
    
    /* See if need to go to the box to get exposure status */
    exp_pic = exp_color = exp_line = -1 ;   /* Assume we are NOT exposing */
    if ( want_exposure_label )
    {   /* Yes, inquire it */
        if ( DP_GetPrinterStatus(dp, INQ_STATUS | INQ_BUFFER) )
        {   /* Error getting status */
            print_err_and_exit( dp ) ;
        }

        /* Are we exposing ? */
        if ( exposing = DP_EXPOSINGIMAGE(dp) )
        {   /* Yes, Figure out picture number, color, and line */
            exp_pic   = last_xmit_pic - DP_IMAGESINQUEUE(dp) + 1 ;
            exp_color = DP_CURRENTCOLOR(dp)  ;
            exp_line  = DP_LINES_EXPOSED(dp) ;
        }
    }

    printf("\nSb: %2.2x %2.2x Queue: %d, color: %d line: %d\n", (unsigned long) dp->ucStatusByte0, (unsigned long) dp->ucStatusByte1, DP_IMAGESINQUEUE(dp), DP_CURRENTCOLOR(dp), DP_LINES_EXPOSED(dp));

    /* Do the exposure printing  */
    if ( exp_pic != -1 )
    {   /* Figure out how full the buffer is */
        percent_full = (float) 1.0 -
            ((float)DP_FREE_BUFFER(dp) / (float) DP_TOTAL_BUFFER(dp));
        percent_full *= 100.0 ;     /* Convert to percentage */
        printf ("  Exposing P#%d,%s-%4d Buf:%3.0f%%",
                        exp_pic, color_str[exp_color], exp_line,
                        percent_full                        ) ;
    printf("\nused: %d, total: %d\n", DP_FREE_BUFFER(dp), DP_TOTAL_BUFFER(dp));
    }
    else
    {   printf ("                                 " ) ;
    }
    
    /* All done,  Get to beginning of the line */
    printf ("\r") ;
    fflush(stdout);
    
    return exposing ;
}

/* Form Feed  */
/* firmware_earlier_than

This function returns TRUE if the firmware version of the filmprinter is
earlier than "reqd_version".

It returns FALSE if the film printer is the same or a later revision.

28-sep-90  tc   Initial
 4-oct-90  tc   DeANSI C'ed and collapse 2 structs to DP_DATA
*/

int firmware_earlier_than (dp, reqd_version )
    DP_DATA *dp ;
    int reqd_version ;      /* Version number to test against */
{
    /* Local declarations */
#define NUM_DIGITS_IN_REV   3

    char    *rom_ver_str ;  /* Points to version digits */
                         /* Local copy of version digits */
    char    local_rom_ver_str[NUM_DIGITS_IN_REV+1] ;
    int     filmprinter_rom_version ;
    int     digit ;                         /* working */

    
    /* Ask the film printer to tell us the version */
    if ( DP_GetPrinterStatus(dp,INQ_OPTIONS ) )
    {   /* Error getting status */
        printf ("\nError getting status!\n") ;
        print_err_and_exit( dp ) ;
    }

    /* Ok, the string from inquire looks like:
            Version 160
    Only work on the last 3 chars of the string so as to skip
        the Version number lead-in stuff
    Convert all non-numeric values to 9, so that 17X will
        turn into 179 and indicate it is more recent than 170.
    */

    /* Copy the last three digits into a local buffer */

            /* Points at start of 160 (in our example) */
    rom_ver_str = DP_ROM_VERSION(dp) +
            strlen( DP_ROM_VERSION(dp) )  - NUM_DIGITS_IN_REV ;
    strcpy ( local_rom_ver_str, rom_ver_str) ;
    
    /* Convert non-numerics into 9 */
    for (digit=0 ; digit < NUM_DIGITS_IN_REV ; digit++)
    {   if ( !isdigit( local_rom_ver_str[digit] ) )
        {       /* Non-numberic, convert it */
                local_rom_ver_str[digit] = '9' ;
        }
    }
    
    /* Convert string to an int */
    filmprinter_rom_version = atoi ( local_rom_ver_str ) ;
    
    /* Give 'um the answer */
    return ( filmprinter_rom_version < reqd_version ) ;
}

 
/* stricmp is a function which is available in MSC on DOS.  A useful
   function which many of the demo apps use to check command line args.
   This is not an ANSI C function so it's included here.
       
   stricmp- Returns a value less than, equal to, or greater than 0, depending
            on whether the string pointed to by string1 is less than, equal
            to, or greater than the string pointed to by string2. The
            comparison is done without regard to case
*/
int stricmp( string1, string2)
char *string1, *string2 ;
{   /* just use strnicmp with a large count */
    return( strnicmp( string1, string2, 100 )) ;
}



/* strnicmp is similiar to stricmp except that a maximum character count
   is provided.  A zero value is returned if the first count characters
   are the same without regard to case
*/       
int strnicmp( string1, string2, count )
char *string1, *string2 ;
int count ;
{
    while( tolower( *string1 ) == tolower( *string2 ))
    {
        if ( (!(*string1)) || (!(--count)) )
            return(0) ;     /* null found, end of string, they compare */
        string1++ ;
        string2++ ;
    }
    return( (int)( *string1 - *string2 ) ) ;
}

