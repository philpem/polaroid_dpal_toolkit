/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPNCR.C */
/*  *3     3-MAY-1991 12:12:55 BROGNA "" */
/*  *2    18-JAN-1991 04:57:19 BROGNA "MSC 6.0x" */
/*  *1     5-NOV-1990 18:07:57 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPNCR.C */
/*  scdpncr.c

 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
    
    Low level scsi functions for the NCR ADP-31S interface board running
    on a PC.
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
        
    Refer to scsiio.doc for a detailed description of how to build the
    low level scsi io routines for a new scsi interface.


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

#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif

#include "dpalette.h"
#include "sclowlvl.h"
#include <dos.h>

int DP_scsi_driver(char *, int, char *, int, char *, int) ;
void DP_scsi_reset(void) ;
int DP_host_retrieve_scsi_error(DP_DATA *, int) ;
int DP_scsi_setup(DP_DATA *, char **) ;

/*  this structure defines all of the parameters which are required to
    pass to the ncr scsi driver which is accessible via int0x13.  This
    structure is filled in for each scsi transfer.
*/        
static struct {
    char command ;          /* scsi driver command type */
    char id ;               /* scsi id goes in the upper 3 bits */
    char cdblk[12] ;        /* command block, we use 6 but 12 are available */
    char return_code ;      /* driver return status */
    char target_status ;    /* target status */
    int  data_seg ;         /* segment of the data block */
    int  data_off ;         /* offset of the data block */
    int  data_size ;        /* size of the data block */
    int  sense_seg ;        /* segment of the extended sense data area */
    int  sense_off ;        /* offset of the extended sense data area */
    int  sense_size ;       /* size of the extended sense data area */
} ncr ;

int DP_scsi_driver( cdb, scsi_id, data_ptr, data_size, extended_sense, dir )
char *cdb, *data_ptr, *extended_sense ;
int data_size, scsi_id, dir ;
{
    int X ;
    union REGS regs ;
    char far *farptr ;
    
    /* set the driver command and the LUN values to zero */
    ncr.command = 2 ;       /* '2' says do an extended sense on chkcondition*/
    
    /* put the current scsi_id in the upper 3 bits */
    ncr.id = (char) (( scsi_id << 5 ) & 0xE0) ;

    /* move the CDB into the ncr cdbblk */
    for (X = 0 ; X < CDB_SIZE ; X++ )
        ncr.cdblk[X] = *cdb++ ;

    /* fill in the pointers in the table,  For the ncr, there's a bit of
       bit twiddling required here.  Far pointers in '86 land contain the
       segment, then the offset.  The tables passed to the ncr driver
       want offset, then segment.  We'll cast the pointers as far pointers
       so that there's no concern for the model under which we're compiling.
       Then turn the two chunks of the pointer and put them in the proper
       order
    */

    farptr = data_ptr ;
    ncr.data_off = (int)( (long)farptr & 0xFFFF ) ;
    ncr.data_seg = (int)( ( (long)farptr >> 16 ) & 0xFFFF ) ;
    ncr.data_size = data_size ;

    farptr = extended_sense ;
    ncr.sense_off = (int)( (long)farptr & 0xFFFF ) ;
    ncr.sense_seg = (int)( ( (long)farptr >> 16 ) & 0xFFFF ) ;
    ncr.sense_size = SENSE_SIZE ;
    
    /* Setting AH to 0xC0 on a call to int 13h will get us to the ncr
       SCSI driver.
    */
    regs.h.ah = 0xc0 ;      /* set up the regs to call the scsi driver */
    regs.h.dl = 0x80 ;
    regs.x.si = (int)&ncr ;

    /* do the call to the ncr scsi driver.... */
    int86( 0x13, &regs, &regs ) ;
    
    /*  return code of 1 says theres no bus breakdown. There may, however,
        have been a check condition status on the command.  If this is so,
        ncr.target_status will contain a CHECK_CONDITION.  And the
        extended sense data will have already been retrieved.
    */            

    if ( ncr.return_code == 1 )
    {
        if ( ncr.target_status  & CHECK_CONDITION )
            return( EXTSENSE_VALID ) ;
        else
            return( SCSI_COMMAND_OK ) ;       /* no errors */
    }

    /*  some sort of scsi bus breakdown or driver error occurred.
        return the ncr driver return code.
    */

    return( (int)ncr.return_code & 0xFF ) ;
}


/* issue a reset pulse on the scsi bus. This function is not called by any
   of the toolkit routines.  It is only included here for completeness in
   case it's needed in the future.
*/       
void DP_scsi_reset()
{
    /* not going to do it here */
}



static struct { int errno ; int err_class ; char *errstring ; } err_msg[] = {
    { 0x02, DPTOOLKITERR,     "Buffer Bounds Error" },
    { 0x03, HARDWERR,         "Correctable Parity Error" },
    { 0x80, DPTOOLKITERR,     "DMA Transfer Buffer Across 64K" },
    { 0x81, TIMEOUT,          "Unable to obtain SCSI Bus" },
    { 0x82, DISCONNECTED,     "Target Selection Timeout" },
    { 0x83, DISCONNECTED,     "SCSI Bus Reset Sent/Received" },
    { 0x84, TIMEOUT,          "Unexpected Disconnection/Bad Phase" },
    { 0x85, TIMEOUT,          "Sense Data Op Failure" },
    { 0x86, HARDWERR,         "Uncorrectable Parity Error" },
    { 0x87, DISCONNECTED,     "SCSIMessage Byte not Sent" },
    { 0x88, DISCONNECTED,     "Message Reject Sent/Received" },
    { 0x89, DPTOOLKITERR,     "Invalid Driver Command Code" },
    { 0x8a, HARDWERR,         "SCSI Adaptor, Diagnostic Failure" },
        
    { 0, DPTOOLKITERR, "Unknown SCSI Driver Error" }       /* end of list */
} ;


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


int DP_host_retrieve_scsi_error( dp, error )
DP_DATA     *dp ;
int error ;
{
    int X ;
    
    for ( X = 0 ; err_msg[X].errno ; X++ )
    {
        if ( err_msg[X].errno == error )
            break ;
    }

    DP_ERRORNUMBER( dp ) = err_msg[X].errno ;
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
            
#define HOST_SCSI_NAME  "NCR ADP31S SCSI"

int DP_scsi_setup( dp, driver_name )
DP_DATA *dp ;
char **driver_name ;
{
    *driver_name = HOST_SCSI_NAME ;
    return( NOERROR ) ;
}


