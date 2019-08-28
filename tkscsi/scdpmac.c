/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPMAC.C */
/*  *2     3-MAY-1991 12:12:42 BROGNA "" */
/*  *1     5-NOV-1990 18:07:37 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPMAC.C */
/*  scdpmac.c

 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
    
    Low level scsi functions for the Macintosh
    The function here is 'glue' for connecting the Digital Palette SCSI
    Toolkit to the host SCSI driver.

    The functions which are required here are:
    DP_scsi_setup()              called to initialize the host SCSI driver
    DP_scsi_driver()             function which performs all SCSI IO
    DP_host_retrieve_scsi_error  function which sets host side SCSI errors
        

    DP_scsi_driver()  is called to perform a SCSI command
    This routine will run through all of the scsi phases. After the
    SCSI command has been completed, the STATUS byte is checked to
    see if the status is CHECK CONDITION.  If so, then a GET EXTENDED
    SENSE command must be issued and the extended sense bytes must be
    filled in.
        
    entry:
        char *cdb             pointer to the Command Description Block ( CDB )
            
        int scsi_id           scsi id of the target device ( 0 to 7 )
            
        char *data_ptr        pointer to an array where data is written/read
                              during the SCSI data phase
                                  
        int data_size         size of the data array which is transfered
                              during the SCSI data phase
                                  
        char *extended_sense  pointer to the extended sense buffer.  This 
                              buffer must be filled with SENSE_SIZE bytes
                              from the Get Extended Sense command if the
                              command returns a CHECK CONDITION status.
                                  
        int dir               direction of the data transfer:
                              SCSI_READ   target to initiator
                              SCSI_WRITE  initiator to target
                              SCSI_NODATA no data phase associated
                                          with this cmd
        
    exit:
        SCSI_COMMAND_OK       all went well
        EXTSENSE_VALID        A CHECK CONDITION status was returned, a
                              Get Extended Sense command was issued, and
                              the extended_sense array contains valid data
        (any other value)     a host driver specific error occured.
        
*/
#include "dpalette.h"
#include "sclowlvl.h"

#ifndef noErr
#define noErr 0
#endif


#define scBadParmsErr   4   /* unrecognized instruction in transfer block */
#define scCommErr       2   /* breakdown in SCSI protocols                */
#define scCompareErr    6   /* data compare error during data read        */
#define scPhaseErr      5   /* phase error: target and initiator not in   */
                            /* agreement                                  */
#define scArbNBErr		3	/* Arbitration failed						  */
#define scBusTOErr		9	/* Bus timeout								  */
#define scComplPhaseErr 10	/* SCSI Complete failed						  */
#define scMgrBusyErr	7	/* SCSI Manager busy						  */
#define scSequenceErr	8	/* operation out of sequence				  */
                              
#define GESerr_offset  20   /* my declaration.  When a SCSI driver error
                               occurrs during a get extended sense, this
                               value is added to the error number to
                               distinguish it from an error during the main
                               command
                            */


/* SCSI Status bits which are returned by the SCSIStat Mac Toolbox routine */
#define SCSI_MSG_Bit_Set    0x0010
#define SCSI_CD_Bit_Set     0x0008
#define SCSI_IO_Bit_Set     0x0004

#define SCSI_CD_IO_Bits     0x000C

/* Macros to determine the current phase of the SCSI bus */
#define Status_Phase(s)     ( ( s & SCSI_CD_IO_Bits ) == SCSI_CD_IO_Bits )
#define DataIn_Phase(s)     ( ( s & SCSI_CD_IO_Bits ) == SCSI_IO_Bit_Set )
#define DataOut_Phase(s)    ( ( s & SCSI_CD_IO_Bits ) == 0 )


/* This is the amount of time we will wait for any SCSI phase change */
/* This time is in system ticks ( 60 per second )                    */
#define PHASE_CHANGE_WAIT  ( 4 * 60 )       /* 4 seconds             */


static struct { int scOpcode ; long scParam1; long scParam2; }
               SCSIInstr[2] ;
               
#define scInc   1
#define scNoInc 2 
#define scStop  7

/* in_link is set to true if we're in the middle of a linked command */
static int in_link = 0 ;

/* mac_scsi_phase_wait() will use the SCSIStat Mac Toolbox routine to
   monitor the phase bits on the SCSI bus.  When the bus leaves the
   command phase it will return.  It will also return if we've ended up
   waiting for PHASE_CHANGE_WAIT ticks.  On exit, the current SCSI bus
   status as provided by the SCSIStat routine is returned.
*/       
static mac_scsi_phase_wait()
{
    unsigned long int First_Tick, Curr_Tick ;
    unsigned int SCSI_Bus_Status ;
    
    First_Tick = TickCount() ;      /* get the current tick count, we'll use
                                       this as a timeout
                                    */
    
    while ( ( SCSI_Bus_Status = SCSIStat()) & SCSI_CD_Bit_Set )
    {
        if ( SCSI_Bus_Status & SCSI_IO_Bit_Set )
            /* have gone into the Status Phase */
            return( SCSI_Bus_Status ) ;
        
        if ( ( Curr_Tick = TickCount()) > ( First_Tick + PHASE_CHANGE_WAIT ))
        {
            /* have to make sure the TickCount hasn't wrapped.  With a 32-bit
               tick count, this will happen about once every 2 1/2 years.
               ( the machine has to be turned on for this amount of time
               though)
            */
            if (( First_Tick + PHASE_CHANGE_WAIT ) <  PHASE_CHANGE_WAIT )
            {
                if ( !( Curr_Tick & 0x80000000 ))
                    return( SCSI_Bus_Status ) ;
            }
            else
                return( SCSI_Bus_Status ) ;
        }
    }
    
    return( SCSI_Bus_Status ) ;
}



/* macscsi_start is called to do everything required to begin a SCSI transfer
*/
static int macscsi_start( CDB, scsi_id )
char *CDB ;
int scsi_id ;
{
    unsigned int my_status ;
    
    if (!in_link)
    {
    
	    my_status = SCSIGet() ;
	    if ( my_status != noErr )
	        return( my_status ) ;
    
	    my_status = SCSISelect( scsi_id ) ;
	    if ( my_status != noErr )
	        return( my_status ) ;
	}
    
    in_link = CDB[5] & LINK_BIT_IN_CDB ;
    
    my_status = SCSICmd( CDB, CDB_SIZE ) ;
    
    return( my_status ) ;
}

static char GES_CDB[6] = { 3, 0, 0, 0, 10, 0 } ;

/* macscsi_end is called to do everything to finish up a SCSI transfer.
   We will do the Status and Message phases.
   If a Check Condition Status is returned, then a Get Extended Sense
   Command is issued and the Extended Sense Error is returned.
*/       
static int macscsi_end( extended_sense, scsi_id )
char *extended_sense ;
{
    unsigned int my_status, scsi_status, scsi_message ;
    unsigned long int scsi_timeout ;
    unsigned int SCSI_Bus_Status ;
    
    scsi_timeout = PHASE_CHANGE_WAIT ;
    
    my_status = SCSIComplete( &scsi_status, &scsi_message, scsi_timeout ) ;
    if ( my_status != noErr )
        return( my_status ) ;
    
    /* check the scsi_status. If it's a CHECK CONDITION, then do a
       REQUEST SENSE command and return the extended sense from the error
    */
    if ( scsi_status == CHECK_CONDITION )
    {
    	/* do an extended sense */
 		my_status = macscsi_start( GES_CDB, scsi_id ) ;
    	if ( my_status != noErr )
            return( my_status + GESerr_offset ) ;

        /* wait for a phase change */
        SCSI_Bus_Status = mac_scsi_phase_wait() ;
        
    	/* Build the SCSIInstr and use it as an arg to SCSIRead */
    	SCSIInstr[0].scOpcode = scNoInc ;
    	SCSIInstr[0].scParam1 = (long)extended_sense ;
    	SCSIInstr[0].scParam2 = (long)SENSE_SIZE ;
    	SCSIInstr[1].scOpcode = scStop ;
    
    	SCSIRead( &SCSIInstr ) ;
    
    	my_status = macscsi_end( "\0", scsi_id ) ;
    	
    	if ( my_status == noErr )
            /* return with an indication that a GES command was performed */
            return( EXTSENSE_VALID ) ;
    		
    	return( my_status ) ;
     } 

     return( SCSI_COMMAND_OK ) ;        /* tell 'um all is well */
    
}


int DP_scsi_driver( cdb, scsi_id, data_ptr, data_size, extended_sense, dir )
char *cdb, *data_ptr, *extended_sense ;
int data_size, scsi_id, dir ;
{
    unsigned int my_status ;
    unsigned int SCSI_Bus_Status ;

    my_status = macscsi_start( cdb, scsi_id ) ;
    if ( my_status != noErr )
        return( my_status ) ;

    mac_scsi_phase_wait() ;             /* wait for a phase change */
    SCSI_Bus_Status = SCSIStat() ;      /* get the current bus status */
    
    if ( DataIn_Phase( SCSI_Bus_Status ))
    {
        /* Build the SCSIInstr and use it as an arg to SCSIRead */
        SCSIInstr[0].scOpcode = scNoInc ;
        SCSIInstr[0].scParam1 = (long)data_ptr ;
        SCSIInstr[0].scParam2 = (long)data_size ;
        SCSIInstr[1].scOpcode = scStop ;
    
        SCSIRead( &SCSIInstr ) ;
    }
    
    else if ( DataOut_Phase( SCSI_Bus_Status ))
    {
        /* Build the SCSIInstr and use it as an arg to SCSIWrite */
        SCSIInstr[0].scOpcode = scNoInc ;
        SCSIInstr[0].scParam1 = (long)data_ptr ;
        SCSIInstr[0].scParam2 = (long)data_size ;
        SCSIInstr[1].scOpcode = scStop ;
    
        SCSIWrite( &SCSIInstr ) ;
    }

    return( macscsi_end( extended_sense, scsi_id ) ) ;
}



/* issue a reset pulse on the scsi bus. This function is not called by any
   of the toolkit routines.  It is only included here for completeness in
   case it's needed in the future.
*/       
void DP_scsi_reset()
{
	SCSIReset() ;
}

/****************************************************************
int DP_host_retrieve_scsi_error( dp, error )

    fills in the error class, number, and message entries in the DP_DATA
    structure for errors which are generated by the host side scsi driver.
        
    entry:
            DP_DATA  *dp      pointer to a structure of type DP_DATA
            int error         Error which was returned by DP_scsi_driver()

    exit:
            The class of the error
*/                

/* The err_msg contains an entry for each error which the Mac SCSI driver
   can generate.  For each entry, the DP_DATA error class and error string
   are defined.
*/
static struct { int errno ; int err_class; char *errstring ; } err_msg[] = {

    { scBadParmsErr, DPTOOLKITERR,  "MAC Driver: Bad Instruction"    },
    { scCommErr,     TIMEOUT,       "Palette not found" },  /*bus breakdown */
    { scCompareErr,  DISCONNECTED,  "Mac Driver: Data compare error" },
    { scPhaseErr,    TIMEOUT,       "Mac Driver: phase error"        },
    { scArbNBErr,    DISCONNECTED,  "Mac Driver: Arbitration Failure"},
    { scBusTOErr,    DISCONNECTED,  "Mac Driver: Bus Timeout"        },
    { scComplPhaseErr,  TIMEOUT,    "Mac Driver: SCSIComplete Failed"},
    { scMgrBusyErr,   DPTOOLKITERR, "Mac Driver: SCSI Manager busy"  },
    { scSequenceErr,  DPTOOLKITERR, "Mac Driver: Wrong Sequence"     },
        
    { scBadParmsErr + GESerr_offset, DPTOOLKITERR,
                    "GES Driver: Bad Instruction"    },
    { scCommErr + GESerr_offset, TIMEOUT,
                    "GES Driver: SCSI bus breakdown" },
    { scCompareErr + GESerr_offset, DISCONNECTED,
                    "GES Driver: Data compare error" },
    { scPhaseErr + GESerr_offset, TIMEOUT,
                    "GES Driver: phase error"        },
    { scArbNBErr + GESerr_offset, DISCONNECTED,
                    "GES Driver: Arbitration Failure"},
    { scBusTOErr + GESerr_offset, DISCONNECTED,
                    "GES Driver: Bus Timeout"        },
    { scComplPhaseErr + GESerr_offset, TIMEOUT,
                    "GES Driver: SCSIComplete Failed"},
    { scMgrBusyErr + GESerr_offset, DPTOOLKITERR, 
                    "GES Driver: SCSI Manager busy"  },
    { scSequenceErr + GESerr_offset, DPTOOLKITERR,
                    "GES Driver: Wrong Sequence"     },
        
    { 0, DPTOOLKITERR,  "Unknown Mac SCSI Driver Error" }   /* end of list */
} ;

int DP_host_retrieve_scsi_error( dp, error )
DP_DATA  *dp ;
int error ;
{
    int X ;
    
    for ( X = 0 ; err_msg[X].errno ; X++ )
    {
        if ( err_msg[X].errno == error )
            break ;
    }

    DP_ERRORNUMBER( dp ) = error ;
    strcpy( DP_ERRORMESSAGE( dp ), err_msg[X].errstring ) ;
    DP_ERRORCLASS( dp ) = err_msg[X].err_class ;
    
    return( DP_ERRORCLASS( dp )) ;
}



/****************************************************************
DP_scsi_setup( DP_DATA *dp, char *driver_name )

    DP_scsi_setup is called once by the DP_InitPrinter function.  It can
    be used to plug in DMA or interrupt routines which may be needed by
    the host side SCSI driver.  Keep in mind that an application may call
    the InitPrinter function multiple times so that if this function installs
    stuff which should only occur once, it should lock out the installation
    on subsequent calls.
        
    The function should set the driver name and do an init.  If the init
    fails, the error class, error number, and an error message should be
    set into the DP_DATA structure.  The error entries in the DP_DATA
    structure do not need to be touched if no errors occur.
        
    entry:
        DP_DATA  *dp      pointer to a structure of type DP_DATA
            
        char *driver_name pointer to a character string.  The pointer must 
                          be set here to a string which will be strcat'd to
                          the Palette Toolkit Version name. This way, the
                          app can get information about the SCSI driver which
                          is embedded in the Toolkit.
                          ** THIS STRING SHOULD NOT EXCEED 15 CHARS **
            
    exit:
        error class       0 if all went well.  The error class otherwise
        
*/
            
#define HOST_SCSI_NAME  "Macintosh SCSI"

DP_scsi_setup( dp, driver_name )
DP_DATA *dp ;
char **driver_name ;
{
    *driver_name = HOST_SCSI_NAME ;
    return( NOERROR ) ;
}


