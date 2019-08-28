/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPSPRM.C */
/*  *5     3-MAY-1991 12:13:40 BROGNA "" */
/*  *4    18-JAN-1991 22:48:39 GRABEL "changes to run under LSC on the MAC" */
/*  *3    18-JAN-1991 17:20:58 CAMPBELL_T "Fixed film dirty error" */
/*  *2    18-JAN-1991 04:57:52 BROGNA "MSC 6.0x" */
/*  *1     5-NOV-1990 18:08:48 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPSPRM.C */
/* scsndprm.c
  Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation
 
25-sep-90  vg       initial,  used dpsndprm.c from the Centronics toolkit
                              as a starting point
 4-oct-90  vg       modified for the new data structures
18-jan-91  vg       had a bad test for ver170 after the stop print command
18-jan-91  vg       include tkhost.h must preceed LSCV3 conditional test
 */

#include "tkhost.h"

#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif

#define DPSNDPRM_C
#include "dpalette.h"
#include "tksccmds.h"
#include "scdperrs.h"

/* local routine definitions */
static void DP_fill_modesel( DP_DATA *, char * ) ;
int DP_StartExposure(DP_DATA *) ;
int DP_SendPrinterParams(DP_DATA *) ;
int DP_TerminateExposure(DP_DATA *, int) ;


/* a local mode_select buffer is maintained here.  Data from the Pola structs
   are moved here before a mode select command is sent to the Palette
*/

/* DP_StartExposure( dp )
       Send start exposure command and args to Digital Palette.

       Input:   dp - Pointer to structure of type DPDATA

       Output:  False  - No Error.
                True   - Error.
                dp structure will contain Error message and number.
*/

int DP_StartExposure( dp )
DP_DATA      *dp ;
{

    unsigned char mode_select[ SELECT_BUFFER_SIZE ] ;
    
    /* Check if there is an exposure in progress.  If the Digital Palette
      is exposing an image then return a warning to the caller.
    */
    if (DP_ExposureWarning( dp ))
        return(DP_GetPrinterStatus( dp, INQ_ERROR)) ;


    /* send the mode_sense command from data stored in the DP_DATA struct
       before sending the start exposure command.
           
       This is recommended to maintain complete compatibility with the 
       Centronics version of the toolkit.  The Centronics interface has 
       parameters associated with it's Start Exposure command. 
       These parameters are Image Processing (cIp), Image Usage (cIu),
       Buffer Usage (cBu), and Calibration mode (cCal).
       In Palette/SCSI land, these parameters are set when the mode 
       select command is issued in the SendPrinterParams
       function.  The application, however, may have changed one of the
       parameters between the calls to SendPrinterParams and StartExposure.
           
       Therefore, the mode select should be reissued.
    */
    DP_fill_modesel( dp, (char *) mode_select ) ;
    
    if (DP_ERRORCLASS( dp ) = sc_mode_select( mode_select ) )
        return( DP_GetPrinterStatus( dp, INQ_ERROR) ) ;
    if (DP_ERRORCLASS( dp ) = sc_start_exposure())
        return( DP_GetPrinterStatus( dp, INQ_ERROR) ) ;

    return(DP_ERRORCLASS( dp )) ;
}

/* DP_SendPrinterParams(Params, Status)
       Set a local copy of the mode select data from the data in the
       Pola structures.  The actual mode select command is not sent to
       the Palette until a call is made to start the exposure.
       Input:   Params - Pointer to structure of type POLAPARAMS.
                Status - Pointer to structure of type POLASTATUS.
*/
int DP_SendPrinterParams( dp )
DP_DATA    *dp ;
{
    int           i ;
    unsigned char cadj[4] ;
    unsigned char mode_select[ SELECT_BUFFER_SIZE ] ;
    
    DP_fill_modesel( dp, (char *) mode_select ) ;
    
    /* do a scsi mode select command */
    if (DP_ERRORCLASS( dp ) = sc_mode_select( mode_select ) )
        return( DP_GetPrinterStatus( dp, INQ_ERROR) ) ;
    
    /* send the color tables, background mode, and camera adjust params */
    for ( i = 0 ; i <= COLOR_BLUE ; i++ )
    {
        if (DP_ERRORCLASS( dp ) = sc_set_color_table( i, 
                                          dp->ucaCtTable[i] ))
            return(DP_GetPrinterStatus( dp, INQ_ERROR)) ;
    }

    /* Send the Background Mode with parameters  */
    if  ( DP_ERRORCLASS( dp ) = 
      sc_sweep( (unsigned char *) (& (DP_BACKGROUND_MODE(dp)) ) ))
        return(DP_GetPrinterStatus( dp, INQ_ERROR)) ;

    /* Camera Adjust values  */
    for (i = 0 ; i < MAX_NUM_FILMS ; i++)
    {
        /* need to build a local table with four char values: the filmno,
           xoffs, yoffs, and zoom.  This table is what gets passed to the
           sc_camera_adjust function
        */
        cadj[0] = (char)i ;                     /* film number */
        cadj[1] = DP_CAMERA_ADJUST_X( dp, i ) ; /* xoffset     */
        cadj[2] = DP_CAMERA_ADJUST_Y( dp, i ) ; /* yoffset     */
        cadj[3] = DP_CAMERA_ADJUST_Z( dp, i ) ; /* zoom        */
        
        if ( DP_ERRORCLASS( dp ) = sc_camera_adjust( cadj ))
            return(DP_GetPrinterStatus( dp, INQ_ERROR)) ;
    }

    
    return(DP_ERRORCLASS( dp )) ;
}

/* DP_TerminateExposure( dp,  Method)
       Send terminate exposure command Digital Palette.
       Input:   dp     - Pointer to structure of type DPDATA
                Method - False = Normal, True = Abort current Exposure.
       Output:  False  - No Error.
                True   - Error.
       Status structure will contain Error message and number.
*/
int DP_TerminateExposure( dp, Method)
DP_DATA     *dp ;
int          Method ;
{
    
    int     ErrorClassSave ;
    
    if (!TERM_METHOD_LEGAL(Method))
    {
        DP_ERRORCLASS( dp ) = DPTOOLKITERR ;
        DP_ERRORNUMBER( dp ) = TK_BAD_TERM_METHOD ;
        return(DP_GetPrinterStatus( dp , INQ_ERROR)) ;
    }
   
    if ( !Method )  /* do a normal terminate exposure */
        DP_ERRORCLASS( dp ) = sc_terminate_exposure() ;
    else
    {
        /* send the stop print command.  This should flush the palette's
           internal buffer and stop all printing.  The SCSI firmware in
           version has a bug where it doesn't bust through the case where
           the palette buffer is full.  If an exposure has begun with
           Pack Film, the buffer will show empty.  The best we can do
           to get around this and be compatible with future firmware rev's
           as well as the Centronics interface is force wait_for_buffer
           to suppress_buffer_warning and wait for room in the buffer.
        */
        if ( DP_firmware_rev( dp ) == 170 )
        {
            if ( DP_GetPrinterStatus( dp, INQ_STATUS ) )
                return( DP_ERRORCLASS( dp )) ;
            if ( DP_EXPOSINGIMAGE( dp ))
                ErrorClassSave = REMOVEFILM ;
                
            while ( !(DP_FREE_BUFFER( dp )))
            {
                DP_GetPrinterStatus( dp, INQ_BUFFER ) ;
                DP_sleep( 1 ) ;
            } 
        }

        /* issue the stop print */
        DP_ERRORCLASS( dp ) = sc_stop_print()  ;
        
        /* Another inconsistency ( nice way of saying bug ) in the SCSI
           firmware.  Version 170 should, but does not return the correct
           error condition "film dirty".
        */
        if ( ( ErrorClassSave == REMOVEFILM) ||
             ((DP_ERRORCLASS( dp )) && (DP_firmware_rev( dp ) == 170 )))
        {   /* change the error which is returned */
            DP_ERRORNUMBER( dp ) = SCS_FILM_DIRTY ;
            strcpy (DP_ERRORMESSAGE( dp ), "Film is Dirty") ;
            DP_ERRORCLASS( dp ) = HARDWERR ;
        }
    }
            
            
    if ( DP_ERRORCLASS( dp ) )
        return( DP_GetPrinterStatus( dp, INQ_ERROR)) ;
   
    /* Send a warning back to the caller if the film needs to be manually
       removed from the camera.
    */
    DP_ExposureWarning( dp ) ;

    /* If servo mode has been turned off in the Digital Palette it is
       possible that the last call would not have detected an exposure 
       in progress and if so, would fail to return a REMOVEFILM warning
       to the caller.  As a failsafe measure, make one final check.
    */
    if ((!DP_ERRORCLASS( dp )) && (DP_SINGLEIMAGEMODE( dp )))
        DP_ERRORCLASS( dp ) = REMOVEFILM ;
   
    return(DP_GetPrinterStatus( dp, INQ_ERROR)) ;

}

/* DP_fill_modsel()
    Get all of the pertinent data out of the DP_DATA buffer and place it
    in a mode select data buffer.  The mode select data buffer is appropriate
    for sending to the palette via the mode select command.
*/        
static void DP_fill_modesel( dp, mode_select )
DP_DATA *dp ;
char *mode_select ;
{
    
    mode_select[ PARAMETER_BYTE0 ] = 0 ;
    mode_select[ PARAMETER_BYTE1 ] = 0 ;
    mode_select[ PARAMETER_BYTE2 ] = 0 ;
    
    mode_select[ RESERVED_1 ] = 0 ;
    mode_select[ RESERVED_2 ] = 0 ;
    mode_select[ RESERVED_3 ] = 0 ;
    mode_select[ RESERVED_4 ] = 0 ;
    mode_select[ RESERVED_5 ] = 0 ;
    mode_select[ RESERVED_6 ] = 0 ;
    mode_select[ RESERVED_11 ] = 0 ;
    mode_select[ RESERVED_12 ] = 0 ;
    mode_select[ RESERVED_13 ] = 0 ;

    mode_select[ DESCRIPTOR_LENGTH ] = SELECT_DESCRIPTOR_SIZE ;
    mode_select[ FILM_NUMBER ] = 
               DP_DEFAULT_FILM( dp, (DP_CAMERA_CODE( dp ) & 0x7f)) ;

    mode_select[ HRES_MSB ] = (char) (DP_HOR_RES( dp ) / 256) ;
    mode_select[ HRES_LSB ] = (char) (DP_HOR_RES( dp ) % 256) ;
    
    mode_select[ LINE_LENGTH_MSB ] = (char) (DP_LINE_LENGTH( dp ) / 256) ;
    mode_select[ LINE_LENGTH_LSB ] = (char) (DP_LINE_LENGTH( dp ) % 256) ;

    mode_select[ HOFFSET_MSB ] = (char) (DP_HOR_OFFSET( dp ) / 256) ;
    mode_select[ HOFFSET_LSB ] = (char) (DP_HOR_OFFSET( dp ) % 256) ;

    mode_select[ VRES_MSB ] = (char) (DP_VER_RES( dp ) / 256) ;
    mode_select[ VRES_LSB ] = (char) (DP_VER_RES( dp ) % 256) ;
    
    mode_select[ VOFFSET_MSB ] = (char) (DP_VER_OFFSET( dp ) / 256) ;
    mode_select[ VOFFSET_LSB ] = (char) (DP_VER_OFFSET( dp ) % 256) ;

    mode_select[ IMAGE_HEIGHT_MSB ] = (char) (DP_VER_HEIGHT( dp ) / 256) ;
    mode_select[ IMAGE_HEIGHT_LSB ] = (char) (DP_VER_HEIGHT( dp ) % 256) ;

    mode_select[ LTDRK_VALUE ] = DP_LIGHTEN_DARKEN( dp ) ;

    mode_select[ ETIME_RED ] =    DP_RED_EXP_TIME( dp ) ;
    mode_select[ ETIME_GREEN ] =  DP_GREEN_EXP_TIME( dp ) ;
    mode_select[ ETIME_BLUE ] =   DP_BLUE_EXP_TIME( dp ) ;

    mode_select[ LUM_RED ] =   DP_RED_LUMINANT( dp ) ;
    mode_select[ LUM_GREEN ] = DP_GREEN_LUMINANT( dp ) ;
    mode_select[ LUM_BLUE ] =  DP_BLUE_LUMINANT( dp ) ;

    mode_select[ CBAL_RED ] =   DP_RED_COLOR_BALANCE( dp ) ;
    mode_select[ CBAL_GREEN ] = DP_GREEN_COLOR_BALANCE( dp ) ;
    mode_select[ CBAL_BLUE ] =  DP_BLUE_COLOR_BALANCE( dp ) ;
    
    mode_select[ SERVO_MODE ] = DP_SERVO_MODE( dp ) ;

    mode_select[ DATA_CONVERSION_MODE ] = 0 ;
    mode_select[ COMMAND_BUFFER_USAGE ] =   DP_EXP_BUFFER_USAGE( dp ) ;
    mode_select[ IMAGE_ENHANCEMENT_MODE ] = DP_EXP_IMAGE_PROCESSING( dp ) ;
    mode_select[ IMAGE_CONTROL ] =          DP_EXP_IMAGE_USAGE( dp ) ;
    mode_select[ COMMAND_BUFFER_USAGE ] =   DP_EXP_BUFFER_USAGE( dp ) ;
    mode_select[ CALIBRATION_CONTROL ] =    DP_EXP_CALIBRATION( dp ) ;

    /* set the image transfer type in the mode select and send mode select */
    if (DP_IMAGE_COMPRESSION( dp ) == NON_RLE)
        mode_select[ DATA_CONVERSION_MODE ] = 0 ;
    else if ((DP_IMAGE_COMPRESSION( dp ) == RLE) ||
             (DP_IMAGE_COMPRESSION( dp ) == NON_RLE_SEND_AS_RLE) )
        mode_select[ DATA_CONVERSION_MODE ] = 1 ;
    else mode_select[ DATA_CONVERSION_MODE ] = DP_IMAGE_COMPRESSION( dp )  ;
}

