/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPRERR.C */
/*  *5     3-MAY-1991 12:13:19 BROGNA "" */
/*  *4    18-JAN-1991 22:48:23 GRABEL "changes to run under LSC on the MAC" */
/*  *3    18-JAN-1991 17:20:52 CAMPBELL_T "Fixed film dirty error" */
/*  *2    18-JAN-1991 04:57:27 BROGNA "MSC 6.0x" */
/*  *1     5-NOV-1990 18:08:13 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPRERR.C */
/*      scrpterr.c

 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation

    Report an error condition which has been returned by the low level
    SCSI routines.  There are two groups of errors; those which are
    extended sense error codes and those which are returned by the
    host SCSI driver.

        
13-aug-90  vg       initial....
19-sep-90  vg       generalized the function to make it host independant
 5-oct-90  vg       added sclowlvl.h include
18-jan-91  vg       was improperly classing the film_dirty error as a
                    REMOVEFILM error.  It should be a hardware error
18-jan-91  vg       mb added a test for LSCV3.  An include of tkhost.h must
                      preceed this
*/
#include "tkhost.h"

#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif

#include "dpalette.h"
#define SCDPRERR_C
#include "tksccmds.h"
#include "scdperrs.h"
#include "errno.h"


int DP_retrieve_scsi_error(DP_DATA *, int, char *) ;

/* The err_msg struct contains the additional sense bytes from the extended
   sense command and a corresponding string which describes the error
*/       
static struct {  int errno ;  char *errstring ;}   err_msg[] = {

    { SCS_NO_ADD_SENSE,          "No Additional Sense" },
    { SCS_DIAG_FAILURE,          "PowerOn Diag Failure" },
    { SCS_MEMORY_FAILURE,        "PowerOn Diag, Memory Failure" },
    { SCS_VIDEO_BUFFER_FAILURE,  "PowerOn Diag, Video Buffer Failure" },
    { SCS_VIDEO_DATA_FAILURE,    "PowerOn Diag, Video Data Failure" },
    { SCS_GENERAL_IO_FAILURE,    "PowerOn Diag, IO Failure" },
    { SCS_CHECKSUM_ERROR,        "Checksum Error" },
    { SCS_POWER_ON_FAILURE,      "PowerOn Failure" },
    { SCS_FILTER_WHEEL_JAM,      "Filter Wheel Jam" },
    { SCS_BAD_FILTER_POSITION,   "Bad Filter Position" },
    { SCS_FILM_DIRTY,            "Film is Dirty" },
    { SCS_CAL_ERR0,              "Calibration Error 0" },
    { SCS_CAL_ERR1,              "Calibration Error 1" },
    { SCS_CAL_ERR2,              "Calibration Error 2" },
    { SCS_CAL_ERR3,              "Calibration Error 3" },
    { SCS_CAL_ERR4,              "Calibration Error 4" },
    { SCS_CAL_ERR5,              "Calibration Error 5" },
    { SCS_CAL_ERR6,              "Calibration Error 6" },
    { SCS_CAL_ERR7,              "Calibration Error 7" },
    { SCS_CAL_ERR8,              "Calibration Error 8" },
    { SCS_UNKNOWN_HWERR,         "Unknown Hardware Error" },
    { SCS_INVALID_CDB,           "Invalid CDB" },
    { SCS_INVALID_CMD_OPCODE,    "Invalid Command OpCode" },
    { SCS_RESERVED_BIT_SET,      "Reserved Bit Set" },
    { SCS_UNSUPPORTED_FUNCTION,  "Unsupported Function" },
    { SCS_REQ_LENGTH_INVALID,    "Requested Length Invalid" },
    { SCS_RGB_COLOR_INVALID,     "RGB Color Invalid" },
    { SCS_FLAG_LINK_ERROR,       "Flag/Link Bit Error" },
    { SCS_PRINT_NO_START,        "Print issued w/no Start" },
    { SCS_PRINT_BAD_LENGTH,      "Print Cmd, Bad Length" },
    { SCS_TERMINATE_NO_START,    "Terminate Issued w/no Start" },
    { SCS_COLOR_TAB_INVALID,     "Color Table Invalid" },
    { SCS_INVALID_LUN,           "Invalid LUN" },
    { SCS_START_NO_TERMINATE,    "Start w/no previous Terminate" },
    { SCS_INVALID_FIELD,         "Invalid Field" },
    { SCS_INVALID_FUNCTION,      "Invalid Function" },
    { SCS_LENGTH_OUT_OF_RANGE,   "Length Out of Range" },
    { SCS_FILMNO_NOT_AVAILABLE,  "Film Number not Available" },
    { SCS_FILMNO_OUT_OF_RANGE,   "Film Number Out of Range" },
    { SCS_HRES_OUT_OF_RANGE,     "Horizontal Resolution Out of Range" },
    { SCS_HOFFS_OUT_OF_RANGE,    "Horizontal Offset Out of Range" },
    { SCS_LINE_LENGTH_OUT_OF_RANGE, "Line Length Out of Range" },
    { SCS_VRES_OUT_OF_RANGE,     "Vertical Resolution Out of Range" },
    { SCS_VOFFS_OUT_OF_RANGE,    "Vertical Offset Out of Range" },
    { SCS_LUMRED_OUT_OF_RANGE,   "Luminant Red Out of Range" },
    { SCS_LUMGRN_OUT_OF_RANGE,   "Luminant Green Out of Range" },
    { SCS_LUMBLU_OUT_OF_RANGE,   "Luminant Blue Out of Range" },
    { SCS_CBALRED_OUT_OF_RANGE,  "Color Balance Red Out of Range" },
    { SCS_CBALGRN_OUT_OF_RANGE,  "Color Balance Green Out of Range" },
    { SCS_CBALBLU_OUT_OF_RANGE,  "Color Balance Blue Out of Range" },
    { SCS_EXPTIMRED_OUT_OF_RANGE,"Exposure Time Red Out of Range" },
    { SCS_EXPTIMGRN_OUT_OF_RANGE,"Exposure Time Green Out of Range" },
    { SCS_EXPTIMBLU_OUT_OF_RANGE,"Exposure Time Blue Out of Range" },
    { SCS_LITDRK_OUT_OF_RANGE,   "Lighten/Darken Value Out of Range" },
    { SCS_DCONV_OUT_OF_RANGE,    "Data Conversion Mode Out of Range" },
    { SCS_EXEMODE_OUT_OF_RANGE,  "Execution Mode Out of Range" },
    { SCS_IMGENH_OUT_OF_RANGE,   "Image Enhancement Mode Out of Range" },
    { SCS_CAMADJ_INVALID_PARAMETER, "Camera Adjust Parameter Invalid" },
    { SCS_PRINT_BADLINENO,       "Print Command, Bad Line Number" },
    { SCS_CALCTRL_OUT_OF_RANGE,  "Calibration Control Out of Range" },
    { SCS_IMGCTRL_OUT_OF_RANGE,  "Image Control Out of Range" },
    { SCS_FILMTAB_BADDATA,       "Film Table, Bad Data" },
    { SCS_FILMTAB_BADSIZE,       "Film Table, Bad Size" },
    { SCS_SWEEPMODE_BAD,         "SweepMode, Bad Parameter" },
    { SCS_IMGHEIGHT_OUT_OF_RANGE,"Image Height Out of Range" },
    { SCS_SERVO_OUT_OF_RANGE,    "Servo Mode Parameter Out of Range" },
    { SCS_POWER_ON,              "Just Powered On" },
    { SCS_BAD_MESSAGE_RETRY,     "Bad Message Retry" },
    { SCS_MESSAGE_PARITY_ERROR,  "Message Parity Error" },
    { SCS_INT_PARITY_ERROR,      "Internal Parity Error" },
        
    { 0, "Unknown Extended Sense Error" }
} ;

/* The msbyte of the extended sense indicates the error type.  An illegal
   request is indicated if the upper byte is 25hex
*/       
#define ILLEGAL_REQUEST  0x2500

/* int DP_retrieve_scsi_error( dp, error, extended_sense )

    dp      pointer to a struct of type DP_DATA.  The error number, code,
            and message in this structure will be filled in.
                
    error   The type of error which occurred.  Can be one of the following:
                SCSI_COMMAND_OK     no errors occurred.outine should
                EXTSENSE_VALID      An error was reported by the Palette and
                                    a Get Extended Sense command was issued.
                                    The contents of the extended_sense buffer
                                    are valid.
                (any other value)   The error is a host specific SCSI driver
                                    error.  The error class, number, and
                                    message in this case is filled in by a
                                    host specific error reporting routine.
                                        

    extended_sense  pointer to a character array.  This array contains
                    extended sense data if error == EXTSENSE_VALID
*/


int DP_retrieve_scsi_error( dp, error, extended_sense )
DP_DATA  *dp ;
int error ;
char *extended_sense ;
{
    int X, extsense ;
    unsigned char *p;
    int i;

    if ( error == SCSI_COMMAND_OK )
    {
        DP_ERRORNUMBER( dp ) = 0 ;
        strcpy( DP_ERRORMESSAGE( dp ), "No Errors Reported") ;
        DP_ERRORCLASS( dp ) = NOERROR ;
    	return( NOERROR ) ;
    }

#ifdef DEBUG
    printf("extended_sense: ");
    p = (unsigned char *) extended_sense;
    for(i=0; i<10; i++) printf("%2.2X ", (int) p[i]);
    printf("\n");
    printf("errno: %d\n", errno);
#endif
    
    if ( error != EXTSENSE_VALID )
        /* some sort of host SCSI driver error occurred */
        return( DP_host_retrieve_scsi_error( dp, error )) ; 
    
    if ( EOM_TEST( extended_sense ) )
    {
        DP_ERRORNUMBER( dp ) = EOM_ERRNUMBER ;
        strcpy( DP_ERRORMESSAGE( dp ), EOM_MESSAGE ) ;
        DP_ERRORCLASS( dp ) = EOM_ERRCLASS ;
    	return( EOM_ERRCLASS ) ;
    }
    
    /* This is an extended sense error, search for a match in the err_msg
       structure and print the error description
    */
    
    /* construct the extended sense bytes in extsense */
    extsense = (int)((extended_sense[8] << 8 ) & 0xFF00) 
             | (int)( extended_sense[9] & 0xFF ) ;
         
    for ( X = 0 ; err_msg[X].errno ; X++ )
    {
        if ( err_msg[X].errno == extsense )
            break ;
    }
    
    /* In the dp structure, the error number is set to the error returned
       by the low level scsi function and the error message comes from
       the err_msg structure.  The error class is set to one of two values,
       LOGICERR or HARDWERR.  If the sense key is set to "Illegal Request"
       ( high byte of 'error' is 0x25 ), then the error class is LOGICERR.
       Otherwise, the errorclass is HARDWERR.
    */
    
    DP_ERRORNUMBER( dp ) = err_msg[X].errno ;
    strcpy( DP_ERRORMESSAGE( dp ), err_msg[X].errstring ) ;
    if ( ( extsense & 0xFF00 ) == ILLEGAL_REQUEST )
        DP_ERRORCLASS( dp ) = LOGICERR ;
    else
        DP_ERRORCLASS( dp ) = HARDWERR ;
        
    return( DP_ERRORCLASS( dp ) ) ;
    
}

