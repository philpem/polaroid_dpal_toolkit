/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPASPI.C */
/*  *2     8-AUG-1991 15:00:52 BIELEFELD "updating" */
/*  *1    20-JUN-1991 16:29:55 BIELEFELD "Adding the ASPI SCSI driver to the toolkit." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPASPI.C */

/*  scdpaspi.c

    Copyright(c) Polaroid Corporation, 1991
    
    Low level scsi functions for the ASPI interface running on a PC.
    The function here is 'glue' for connecting the Digital Palette SCSI
    Toolkit to the host SCSI ASPI driver/shell.

    The functions which are required here are:
    DP_scsi_setup()              called to initialize the host SCSI driver
    DP_scsi_driver()             function which performs all SCSI IO
    DP_host_retrieve_scsi_error  function which sets host side SCSI errors
        
        
    DP_scsi_driver()  is called to perform a SCSI command
    This routine will run through all of the scsi phases. After the
    SCSI command has been completed, the ASPI_STATUS and target_status 
    is checked to see if the status is an ASPI error or target 
    CHECK_CONDITION.  If so, the ASPI mgr will automaticaly obtain the
    extended sense data and pass it in pointer extended_sense.
        
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
        EXTSENSE_VALID        A CHECK CONDITION status was returned, the
                              ASPI shell is holding the extended sense, and
                              the extended_sense pointer contains a valid 
                              data a host driver specific error.

			JB	initial...
*/


/* Include Files */

#include <dos.h>
#include <stdio.h>
#include <memory.h>
#include "dpalette.h"
#include "sclowlvl.h"
#include "tkinquir.h"
#include "aspi.h"

/* Function Prototypes */

int  DP_scsi_driver(char *, int, char *, int, char *, int) ;
void DP_scsi_reset() ;
int  DP_host_retrieve_scsi_error(DP_DATA *, int) ;
int  DP_scsi_setup(DP_DATA *, char **) ;

/* Constants */

#define	NO	0
#define YES 1
#define INACTIVE 0
#define ACTIVE 1
#define CHECK_CONDITION 2

/* Statics */

static char GES_CDB[6] = { 3, 0, 0, 0, 10, 0 } ;



int DP_scsi_driver( cdb, scsi_id, data_ptr, data_size, extended_sense, dir )
char *cdb, *data_ptr, *extended_sense ;
int data_size, scsi_id, dir ;
    {
    unsigned char adir;
    char  ha_status, target_status ; 
    int   aspi_status ;

	 
	 if ( dir == SCSI_READ )
        adir = 1;
    else
        adir = 0;
        
    aspi_status = aspi_cmd ((unsigned char)0,           /* host adapter id */
                            (unsigned char)scsi_id,     /* target scsi id */
                            (unsigned char)0,           /* lun number */
                            cdb,                        /* command block */
                            (unsigned char)6,           /* cdb size */
                            data_ptr,                   /* data buffer */
                            (unsigned long)data_size,   /* data size */
                            adir,                       /* READ/WRITE */
                            extended_sense,             /* sense buffer */
                            (unsigned char)SENSE_SIZE,  /* sense size */
                            &ha_status,                 /* host adptr stat */
                            &target_status);             /* target status */

						
/*    aspi_error(cdb, &aspi_status, &target_status, &ha_status); */
	
    /* --- all's well? --- */
    if (aspi_status == 1)
        return( SCSI_COMMAND_OK ) ;
	
	/* --- we have a check condition? --- */
    else if (target_status == CHECK_CONDITION)    
       return (EXTSENSE_VALID);
	
    /* --- Something wrong happened, we got an error in the transfer --- */
    else if ((aspi_status > 1) && (target_status != CHECK_CONDITION))
        {   
        /* we got a timeout or host adaptor error */
/*      aspi_error(cdb, &aspi_status, &target_status, &ha_status); */
        }
            
    return( aspi_status ) ;
    }



/* issue a reset pulse on the scsi bus. This function is not called by any
   of the toolkit routines.  It is only included here for completeness in
   case it's needed in the future.
*/       
void DP_scsi_reset()
{
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

    {   -3, TIMEOUT,       "File spec ASPIMGR$ could not be closed." },
    {   -2, TIMEOUT,       "Unable to obtain ASPI-MGR entry pointer." },
    {   -1, TIMEOUT,       "File spec ASPIMGR$ could not be opened." },
    {    1, NOERROR,       "SCSI request completed without error." },
    {    2, TIMEOUT,       "SCSI request aborted by host." },
    {    3, TIMEOUT,       "Unable to abort SCSI request." },
    {    4, TIMEOUT,       "SCSI request completed with error." },
    { 0x80, DPTOOLKITERR,  "Invalid SCSI request." },
    { 0x81, DPTOOLKITERR,  "Invalid host adapter number." },
    { 0x82, HARDWERR,      "SCSI device not installed." },
    { 0x83, HARDWERR,      "Unable to detect a target device." },
        
    {    0, NOERROR,       "SCSI request in progress" }
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
            
#define HOST_SCSI_NAME  "ASPI SCSI"

DP_scsi_setup( dp, driver_name )
DP_DATA *dp ;
char **driver_name ;
    {
    char aspi[16];
    char ha[16];
    char scsi_id, ha_id;
    asrb_ha_unique ha_unique;
    int ha_tot, dev, dev_cnt;
    int aspi_status;
	 
    *driver_name = HOST_SCSI_NAME ;

    dev_cnt = 0;
	 
    /* --- Make sure there is an ASPI.SYS driver/shell out there --- */
    /* --- While we are at it get the total number of host adapters. --- */
    if ((aspi_status = 
        aspi_host_adapter_inquiry ((char)0, &ha_tot, NULL, aspi, ha, &ha_unique)) != 1)
        return(aspi_status);			/* No ASPI Shell!!! */
	
    /* --- Poll every device number for each host adapter. --- */
    for (ha_id = 0; ha_id < ha_tot; ha_id++)
        if (aspi_host_adapter_inquiry (ha_id, NULL, NULL, aspi, ha, NULL) == 1)
            {
/*          printf ("	SCSI_adapter No: %d\n", ha_id);
            printf ("	ASPI driver:     %s\n",aspi);
            printf ("	Host adapter:    %s\n",ha);  
*/
            for (scsi_id = 0; scsi_id < 7; scsi_id++)
                {
                if (aspi_dev_type(ha_id, scsi_id, 0, &dev) == 1)
                    {
                    dev_cnt++;
/*                  printf("Ha %d, Device %d, Type %d\n",ha_id,scsi_id,dev);
*/
                    }
	
                else
                    {
/*                  printf("Ha %d, Device %d NOT CONNECTED.\n",ha_id,scsi_id);
*/
                    }
                }
            }
    if (dev_cnt)
        return( NOERROR ) ;
	 else
        return(0x83);     /* nothing out there */ 
    }

