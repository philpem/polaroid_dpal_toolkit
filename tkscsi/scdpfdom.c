/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPFDOM.C */
/*  *7     8-MAY-1991 19:39:12 GRABEL "moved id check to tk body" */
/*  *6     3-MAY-1991 12:12:10 BROGNA "" */
/*  *5     9-APR-1991 10:49:51 BIELEFELD "SCSI supports multiple non-consecutive addressed palettes" */
/*  *4     1-APR-1991 18:02:28 BROGNA "Toolkit enhancments" */
/*  *3    25-MAR-1991 16:10:27 BROGNA "Multiple unit ability" */
/*  *2    18-JAN-1991 17:20:46 CAMPBELL_T "Released to CMS for real" */
/*  *1    10-DEC-1990 11:51:28 BROGNA "CMS placeholder for the real scdpfdom.c" */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPFDOM.C */
/*  scdpfdom.c

    Copyright(c) Polaroid Corporation, 1990
    
    Low level scsi functions for the Future Domain interface board running
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

			vg	initial...
18-mar-91	jb	expanded DP_scsi_setup() to run multiple printers.
17-apr-91   vg  moved the check of all scsi id's into the toolkit
*/


#include "dpalette.h"
#include "sclowlvl.h"
#include "tkinquir.h"
#include <dos.h>

/*  this structure defines all of the parameters which are required to
    pass to the oem scsi function. This structure is filled in for each 
    scsi transfer.
        
    Note:  The Future Domain oem object file, as supplied by Future Domain,
           is compiled with the /Zp compile switch.  This switch forces
           structures to be packed.  We won't force this in the toolkit
           but the oemtable needs to be packed.  This can be done with
           the pack pragma.  Refer to the Microsoft C compiler doco for
           more data on the pack pragma.  With the pack pragma, we can
           force the packing of the oemtable structure to keep the oemscsi
           function happy, then turn the pack switch off so that packing
           of other structures reverts to the prevailing mode.
*/  

#pragma pack(1)         /* force structure packing to a 1 byte boundary */

struct oemtable
{
    unsigned char (far *cmdblock) ;
    unsigned char (far *datablock) ;
    long expectlen ;
    long actuallen ;
    int timeout ;
    int blkfactora ;
    int blkfactorb ;
    unsigned char scsistatus ;
    unsigned char scsimsg ;
    unsigned char scsibits ;
    unsigned char scsiaddress ;
    unsigned char scsiparity ;
    int adaptererr ;
    } oem ;

#pragma pack()          /* reverts structure packing to the prevailing mode */


void DP_scsi_reset() ;

static char GES_CDB[6] = { 3, 0, 0, 0, 10, 0 } ;

/* The scsibits entry returns with the scsi bus status bits when a timeout
   condition occurs.  The bits in this char are: (upper 3 bits not used)
*/
#define SCSI_Request_Bit    0x20
#define SCSI_CmdData_Bit    0x10
#define SCSI_IO_Bit         0x04
#define SCSI_Message_Bit    0x02
#define SCSI_Busy_Bit       0x01

#define any_SCSI_Bus_Bits_Set   0x3F        /* any of the SCSI bits set */

#define OEM_TIMEOUT 8

/* The error condition returned by the oem routine conflicts with the
   normal error codes (eg, SCSI_COMMAND_OK and EXTSENSE_VALID ).  If the
   error which is returned by the driver is positive, then an offset is
   added to avoid conflict.  This offset is accomodated in the error
   reporting routine
*/
#define OEM_ERROR_OFFSET 0x10


int DP_scsi_driver( cdb, scsi_id, data_ptr, data_size, extended_sense, dir )
char *cdb, *data_ptr, *extended_sense ;
int data_size, scsi_id, dir ;
{
    int X ;
    char far *farptr ;

    /* put the current scsi_id in the oem structure */
    oem.scsiaddress = scsi_id ;

    /* set the CDB pointer in the oem struct */
    oem.cmdblock = cdb ;

    oem.datablock = data_ptr ;
    oem.expectlen = (long) data_size ;
    
    if ( dir == SCSI_READ )
    {
        oem.blkfactora = 1 | 0x8000 ;
        oem.blkfactorb = data_size - 1 ;
    }
    else if ( dir == SCSI_WRITE )
    {
        oem.blkfactora = 1 | 0x8000 ;
        oem.blkfactorb = data_size - 1 ;
    }
    else
    {
        oem.blkfactora = 1 | 0x8000 ;
        oem.blkfactorb = data_size - 1 ;
    }
        
        
    oem.scsiparity = 0 ;
    oem.timeout = OEM_TIMEOUT ;
    
    oemscsi( &oem ) ;

    if ( !oem.adaptererr && !( oem.scsistatus & CHECK_CONDITION ))
        return( SCSI_COMMAND_OK ) ;
    
    /* Something wrong happened, either we got an error in the transfer
       or the Palette reported a Check Condition
    */
    if ( oem.adaptererr < 0 )
    {   /* we got a timeout or host adaptor error */
        /* if any of the SCSI bits on the bus are active, we should issue
           a reset to clear the bus
        */
        if ( oem.scsibits & any_SCSI_Bus_Bits_Set )
            DP_scsi_reset() ;
        return( oem.adaptererr ) ;
    }
            
    /* if oem.adaptererr is greater than zero, it's because the data transfer
       was not complete.  This may well be because the palette skipped the
       data phase because there was an outstanding error.
    */
    if ( !(oem.scsistatus & CHECK_CONDITION) )
        return( oem.adaptererr + OEM_ERROR_OFFSET ) ;
    
    /* We have a check condition, issue the Get Extended Sense Command */
    /* put the current Phys_id in the oem structure */
    oem.scsiaddress = scsi_id ;

    /* set the CDB pointer in the oem struct */
    oem.cmdblock = GES_CDB ;

    oem.datablock = extended_sense ;
    oem.expectlen = (long) SENSE_SIZE ;
    oem.blkfactora = 1 ;
    oem.blkfactorb = 1 ;
    oem.scsiparity = 0 ;
    oem.timeout = OEM_TIMEOUT ;
    
    oemscsi( &oem ) ;
    
    if ( !oem.adaptererr )
        return( EXTSENSE_VALID ) ;

    if ( oem.adaptererr > 0 )
        oem.adaptererr += OEM_ERROR_OFFSET ;
    else
    {   /* error is negative which indicates a timeout condition */
        /* if any of the SCSI bits on the bus are active, we should issue
           a reset to clear the bus
        */
        if ( oem.scsibits & any_SCSI_Bus_Bits_Set )
            DP_scsi_reset() ;
    }
        
    
    return( oem.adaptererr ) ;
}


/* issue a reset pulse on the scsi bus. This function is not called by any
   of the toolkit routines.  It is only included here for completeness in
   case it's needed in the future.
*/       
void DP_scsi_reset()
{
    oemreset() ;
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

static struct { int errno ; int err_class; char *errstring ; } err_msg[] = {

    { -1, TIMEOUT,  "Palette not found" },  /* Timeout in bus free phase */
    { -2, TIMEOUT,  "Palette not found" },  /* Timeout during Selection  */
    { -3, TIMEOUT,  "SCSI Driver: Timeout during Command Phase"    },
    { -4, TIMEOUT,  "SCSI Driver: Timeout during Data Phase"    },
    { -5, TIMEOUT,  "SCSI Driver: Timeout during Status Phase"    },
    { -6, TIMEOUT,  "SCSI Driver: Timeout during Message Phase"    },
    { -7, TIMEOUT,  "SCSI Driver: Parity Error" },
	{ -9, DPTOOLKITERR, "Attempting to access a non-palette SCSI device"},
	{ -10, HARDWERR, "No Palette was found" },
    { -16, DISCONNECTED, "Error locating TMC host adapter" },
    {  1 + OEM_ERROR_OFFSET, TIMEOUT, "Buffer overflow" },
    {  2 + OEM_ERROR_OFFSET, TIMEOUT, "Buffer underflow" },
        
    { 0, DPTOOLKITERR,  "Unknown FD SCSI Driver Error" }   /* end of list */
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
            
        char **driver_name pointer to a character string.  The pointer must 
                           be set here to a string which will be strcat'd to
                           the Palette Toolkit Version name. This way, the
                           app can get information about the SCSI driver which
                           is embedded in the Toolkit.
                           ** THIS STRING SHOULD NOT EXCEED 15 CHARS **
            
    exit:
        error class       0 if all went well.  The error class otherwise
        
----------------------------------------------------------------------------*/
            
#define HOST_SCSI_NAME  "FutDomain SCSI"

/* for the future domain driver, there's no init call.  Simply set the
   driver name and return.
*/

DP_scsi_setup( dp, driver_name )
DP_DATA *dp ;
char **driver_name ;
    {
	int dev;
	
    *driver_name = HOST_SCSI_NAME ;
	
    return( NOERROR ) ;
}
