/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPPS2.C */
/*  *4     3-MAY-1991 12:13:05 BROGNA "" */
/*  *3     9-APR-1991 10:48:12 BIELEFELD "SCSI supports multiple non-consecutive addressed palettes" */
/*  *2     1-APR-1991 18:14:06 BROGNA "Toolkit enhancments" */
/*  *1    26-MAR-1991 11:38:53 BROGNA "PS2 SCSI support" */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPPS2.C */
/*  scdpps2.c

Copyright(c) 1988, 1989, 1990, 1991 Polaroid Corporation
	
	J. Bielefeld	2/18/91
		4/4/91	- Added the ability to search for and run multiple palettes.
    
    Low level scsi functions for the IBM SCSI interface board running
    on a PS-2.
    The function here is 'glue' for connecting the Digital Palette SCSI
    Toolkit to the host SCSI driver.

    The functions which are required here are:
    DP_scsi_driver()              function which performs all SCSI IO
    DP_scsi_adprst()              function which resets the SCSI adapter
                                  during a hardware hang-up
    DP_scsi_reset()               function which resets the SCSI adapter
                                  and SCSI devices
    DP_scsi_setup()               called to initialize the host SCSI driver
    DP_host_retrieve_scsi_error() function which sets host side SCSI errors
    DP_scsi_allocate()            called to allocate a SCSI device
    DP_scsi_deallocate()          called to dealocate a SCSI device
        
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
        
============================================================================*/

#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif

#include "dpalette.h"
#include "sclowlvl.h"
#include "tkinquir.h"
#include <dos.h>

#define READ	    0x8000
#define ERRSTATUS   0x4000
#define RETRY       0x2000
#define SSBIT       0x0400
#define BYPASS      0x0200

#define SNDOTHRCMD  0x249F
#define REQSENSE    0x1C88
#define RESET       0x0400

#define MAX_DEVICES 8

#define DEVICE_ALLOCATED 0x20
#define IBM_FXD_DISK     0x08
#define REMOVABLE_MEDIA  0X04
#define DEFECTIVE_DRIVE  0X02
#define DEV_NOT_POWERED  0X01

#define NO          0
#define YES         1
#define NONE        0xFFFF

#define DO_UNTIL_BREAK for(;;)

int DP_scsi_driver(char *, int, char *, int, char *, int);
int DP_scsi_adptrst(int);
void DP_scsi_reset();
int DP_host_retrieve_scsi_error(DP_DATA *, int);
int DP_scsi_setup(DP_DATA *, char **);
int DP_scsi_allocate(int);
int DP_scsi_deallocate(int);

/*----------------------------------------------------------------------
    this structure defines all of the parameters which are required to
    pass to the IBM SCSI driver which is accessible via int0x4B.  This
    structure is filled in for each scsi transfer.
-----------------------------------------------------------------------*/

typedef struct {
    int  cmd;               /* send other SCSI command */
    int  enable;            /* Enable word */
    int  cmdlen;            /* SCSI command length */
    int  reserved;          /* reserved for IBM */
    long bufAddr;           /* System buffer address */
    long bufCount;          /* System buffer byte count */
    long tsbAddr;           /* Terminate status block address */
    long chain;             /* Optional SCB chain address */
    char scsiCmd[6];        /* SCSI commands */
} PS2_SCB;

static PS2_SCB scb_block;
static PS2_SCB *ps2_SCB = &scb_block;

typedef struct {
    int  SCBstatus;         /* SCB Status */
    int  retryCount;        /* Retry count */
    long residualByteCount; /* Residual Byte Count */
    long residualBufAddr;   /* Residual Byte Address */
    int  statusLen;         /* Device dependent status length */
    int  cmdDeviceStat;     /* Command device status */
    int  cmdDeviceErr;      /* Command device error code */
    int  reserved;
    int  cacheInfo;         /* Cache information word */
    long lastSCBaddr;		/* Last SCB address processed */
} PS2_TSB;

static PS2_TSB tsb_block;
static PS2_TSB *ps2_TSB = &tsb_block;

typedef struct {			/* lookup table to convert the scsi_id or */
	unsigned char SCSI_key;	/* or logical address and find the physical */
	unsigned char Device_Count; /* device address */
	int           Palette;
	} DEV_LOOKUP;

static DEV_LOOKUP SCSI_addr[8];	/* the index will be scsi_id */

static int SCSI_devcnt;
static int SCSI_id;
static int done_once;

static char GES_CDB[6] = {3,0,0,0,10,0};
static char INQ_CDB[6] = {18,0,0,0,63,0};
static char inq_data[9][INQUIRY_BUFFER_SIZE];
static char *inqdptr;


/*----------------------------------------------------------------------
This routine is the main SCSI driver which performs all SCSI IO.

/* NOTE:
	The DP_scsi_driver() routine calls DP_scsi_allocate() and 
DP_scsi_deallocate() routines to in effect mount and unmount the SCSI
device. These routines may be removed to increase the SCSI driver
efficiency. If they are removed, they should be placed elseware in the
application. Never should the application write to a SCSI device without
allocating and exit to DOS without deallocating.
-----------------------------------------------------------------------*/

int DP_scsi_driver( cdb, scsi_id, data_ptr, data_size, extended_sense, dir )
char *cdb, *data_ptr, *extended_sense;
int data_size, scsi_id, dir;

	{
    int X;
    union  REGS  regs;
    struct SREGS sregs;
    char   far   *farptr;
    int          dev_cnt, error, status;


	/* --- check the scsi_id for a valid palette address --- */
	if (SCSI_addr[scsi_id].Palette != YES)
		return(0x0F);
	
	/* --- convert SCSI device ID to device count --- */
	dev_cnt = SCSI_addr[scsi_id].Device_Count;

	/* --- allocate the SCSI device --- */
	if ((status = DP_scsi_allocate(dev_cnt)) > 0)
		return(status);			/* something went wrong */

	DO_UNTIL_BREAK
		{
		/* --- set up the enable bits --- */
		if (dir == SCSI_READ)
			ps2_SCB->enable = READ | ERRSTATUS | RETRY | SSBIT | BYPASS;
		else
			ps2_SCB->enable = ERRSTATUS | RETRY | SSBIT | BYPASS;
    
		/* --- set up the SCSI command --- */
		ps2_SCB->cmd     = SNDOTHRCMD;

		/* --- set up the TSB pointer --- */
		ps2_SCB->tsbAddr = ((long)ps2_TSB & 0xFFFF)+
			(((long)ps2_TSB >>12) & 0xFFFF0);

		/* --- load the SCSI CDB size --- */
		ps2_SCB->cmdlen  = CDB_SIZE;
    
		/* --- move the CDB bytes into the IBM SCB block --- */
		for (X = 0; X < CDB_SIZE; X++)
			ps2_SCB->scsiCmd[X] = *cdb++;
    
		/* --- load the data buffer address --- */
		ps2_SCB->bufAddr = ((long)data_ptr & 0xFFFF)+
			(((long)data_ptr >>12) & 0xFFFF0);

		/* --- load the size of the data buffer --- */
		ps2_SCB->bufCount = (long)data_size;

		/* --- send command SCB --- */
		regs.h.ah = 0x80;                /* BIOS call to SCSI devices */
		regs.h.al = 0x04;                /* Send Device SCB */
		sregs.es  = FP_SEG(ps2_SCB);     /* set up SCB address */
		regs.x.bx = FP_OFF(ps2_SCB);
		regs.h.cl = 0x01;                /* SCB type indicator */
		regs.x.dx = SCSI_addr[dev_cnt].SCSI_key;   /* load device key */
		int86x(0x4b,&regs,&regs,&sregs); /* do it!!! */
//		bios_err(regs.x.ax);
		
		/* --- check to see if we obtained any error on the SCSI bus */
		/* the cflag (carry flag) == 0 no errors --- */
		if (regs.x.cflag == 0)
			{
			status = SCSI_COMMAND_OK; /* no errors */
			break;
			}
		
		/* --- check for what BIOS error we got --- */
		if (regs.x.ax == 0)
			{
			status = SCSI_COMMAND_OK;
			break;
			}

		/* --- if the request ended in error and interrupt status
               indicates that command completed in failure then
               read the Termination Status Block --- */
		if ((regs.h.ah == 0x06) & ((regs.h.al & 0x0F) == 0x0C))
			{
			/* --- Check the Device Status Code in the 
                   Termination Status Block for a Check Condition --- */
			if ((ps2_TSB->cmdDeviceStat & 0x000F) == 0x0002)
				{
				/* --- if a Check Condition is valid then request 
					   extended sense from the device --- */
				for (X = 0; X < CDB_SIZE; X++)
					ps2_SCB->scsiCmd[X] = 0;
				ps2_SCB->enable   = READ | ERRSTATUS | RETRY | SSBIT | BYPASS;
				ps2_SCB->cmd      = REQSENSE;
				ps2_SCB->cmdlen   = 0; 
				ps2_SCB->reserved = 0;
				ps2_SCB->chain    = 0;
				ps2_SCB->tsbAddr  = ((long)ps2_TSB & 0xFFFF) + 
                                    (((long)ps2_TSB >>12) & 0xFFFF0);
				ps2_SCB->bufAddr  = ((long)extended_sense & 0xFFFF) +
                                    (((long)extended_sense >>12) & 0xFFFF0);
				ps2_SCB->bufCount = (long)SENSE_SIZE;
			
				regs.h.ah         = 0x80;   /* BIOS call to SCSI Device */
				regs.h.al         = 0x04;   /* Send device SCB */
				sregs.es          = FP_SEG(ps2_SCB);
				regs.x.bx         = FP_OFF(ps2_SCB);
				regs.h.cl         = 0x00;   /* SCB type indicator */
				regs.x.dx         = SCSI_addr[dev_cnt].SCSI_key; /* dev key */
				int86x(0x4B,&regs,&regs,&sregs);
//				bios_err(regs.x.ax);
			
				/* --- check for errors --- */
				if (regs.x.cflag == 0)
					{
					status = EXTSENSE_VALID;
					break;
					}
				else
					{
					if (regs.h.al == 0)
						{
						status = EXTSENSE_VALID;
						break;
						}
					DP_scsi_adptrst(dev_cnt);
					status = (int)regs.h.ah;
					break;
					}
				}
			}
		else status = (int)regs.h.ah; /* return the error */
		break;
		} /* end DO_UNTIL_BREAK */
	
	if ((error = DP_scsi_deallocate(dev_cnt)) > 0)	/* dealocate the device */
		return (error);
	return (status);
	
	} /* end DP_scsi_driver() */

   
/*------------------------------------------------------------------------
   issue a reset pulse on the scsi bus. This function is not called by any
   of the toolkit routines.  It is only included here for completeness in
   case it's needed in the future.
-------------------------------------------------------------------------*/

int DP_scsi_adptrst(dev_cnt)
    int dev_cnt;
{
union REGS regs;

printf ("---Adapter Reset---\n");
/* --- reset the SCSI adapter --- */
regs.h.ah = 0x80;                /* SCSI BIOS call */
regs.h.al = 0x06;                /* Function = Reset Adapter */
regs.x.dx = SCSI_addr[dev_cnt].SCSI_key;   /* Device Key */
int86(0x4B, &regs, &regs);       /* SCSI interrupt */
// bios_err(regs.x.ax);

/* --- did the reset succeed --- */
if (regs.x.cflag != 0)
	if (regs.h.ah > 0x00)
		printf ("\n\n SCSI ADAPTER FAILED TO RESET\n\n");
return;
}


/*-----------------------------------------------------------------------
Issue a global reset to the adapter and all SCSI attached devices:
------------------------------------------------------------------------*/

void DP_scsi_reset()
   {
   return;
   }


/*--------------------------------------------------------------------------
    Here we have the error strings:
---------------------------------------------------------------------------*/

static struct { int errno ; int err_class ; char *errstring ; } err_msg[] = {
    { 0x01, DPTOOLKITERR, "Invalid BIOS function or parameter" },
    { 0x02, DPTOOLKITERR, "Device not allocated" },
    { 0x03, DPTOOLKITERR, "Device already allocated" },
    { 0x04, DPTOOLKITERR, "Function not allowed" },
    { 0x05, HARDWERR,     "Adapter unusable, adapter reset required" },
    { 0x06, HARDWERR,     "Request ended in error" },
    { 0x07, HARDWERR,     "Adapter reset failed" },
    { 0x08, HARDWERR,     "Adapter reconfiguration failed after reset" },
    { 0x09, HARDWERR,     "Adapter time-out error" },
    { 0x0A, DPTOOLKITERR, "Adapter in use" },
    { 0x0B, HARDWERR,     "Reset adapter before next command" },
    { 0x0C, DPTOOLKITERR, "Request command complete status" },
    { 0x0D, HARDWERR,     "SCSI adapter not attached"},
    { 0x0E, HARDWERR,     "No SCSI printer attached"},
	{ 0x0F, DPTOOLKITERR, "Device accessed is not a palette"},
        
    { 0x00, DPTOOLKITERR, "Unknown SCSI Driver Error" }  /* end of list */
} ;


/*------------------------------------------------------------------------
int DP_host_retrieve_scsi_error( dp, error )

    fills in the error class, number, and message entries in the DP_DATA
    structure for errors which are generated by the host side scsi driver.
        
    entry:
            DP_DATA  *dp      pointer to a structure of type DP_DATA
            int error         Error which was returned by DP_scsi_driver()

    exit:
            The class of the error
---------------------------------------------------------------------------*/

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


/*--------------------------------------------------------------------------
DP_scsi_setup( DP_DATA *dp, char *driver_name )

    DP_scsi_setup is called once for each SCSI device by the DP_InitPrinter 
	function.  It can be used to plug in DMA or interrupt routines which 
	may be needed by the host side SCSI driver.  Keep in mind that an 
	application may call the InitPrinter function multiple times so that 
	if this function installs stuff which should only occur once, it should 
	lock out the installation on subsequent calls.
        
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
----------------------------------------------------------------------------*/
            
#define HOST_SCSI_NAME  "IBM PS/2 SCSI"

int DP_scsi_setup( dp, driver_name )
DP_DATA *dp ;
char **driver_name ;

{
union REGS regs;
struct SREGS sregs;
unsigned char _far *scsi;
int x,dev_count;
int error;

    *driver_name = HOST_SCSI_NAME ;
    
	if (done_once == NO)
		{
		done_once = YES;
		scsi = (unsigned char *)0x0047B; /* check the hardware */
        if ((*scsi & 0x02) == 0)
            {
//          printf ("---> SCSI card not installed <---\n");
            DP_ERRORCLASS(dp) = HARDWERR;
		    return(0x0D);	/* oops! SCSI card not installed */
            }
        else  /* see how meny printer devices are out there? */
            {
            regs.h.ah = 0x80;       /* BIOS call to SCSI device */
            regs.h.al = 0x00;       /* Function = Get device count */
            regs.x.dx = 0x02;       /* Device   = Printer (2) */
            int86(0x4B,&regs,&regs); /* SCSI interrupt */
//          bios_err(regs.x.ax);
		
            if (regs.x.cflag != 0)
                {
//              printf ("---> No printer attached to SCSI <---\n");
                DP_ERRORCLASS(dp) = HARDWERR;
                return(0x0E); /* No printer attached to SCSI */
                }
            }
		
        if ((SCSI_devcnt = (int)regs.h.cl) > 0)	/* store the device count */
		    {
			/* set all device addresses as non-palette until otherwyse */
			for (x = 0; x < 8; x++)
				SCSI_addr[x].Palette = NO;
			SCSI_id = 1;	/* start mapping with scsi_id == 1 */
		    for (dev_count = 1; dev_count <= SCSI_devcnt; dev_count++)
				{
				/* need to check each printer device and make shure they
				   are digital palettes */
				if ((error = DP_scsi_allocate(dev_count)) != 0)
					{
					DP_scsi_deallocate(dev_count);
					return(error);
					}
				ps2_SCB->enable   = READ|ERRSTATUS|RETRY|SSBIT|BYPASS;
				ps2_SCB->cmd      = SNDOTHRCMD;
				ps2_SCB->tsbAddr  = ((long)ps2_TSB & 0xFFFF) + 
					                (((long)ps2_TSB >> 12) & 0xFFFF0);
				ps2_SCB->cmdlen   = CDB_SIZE;
				for (x = 0; x < CDB_SIZE; x++)
					ps2_SCB->scsiCmd[x] = INQ_CDB[x];
				inqdptr           = &inq_data[dev_count][0];
				ps2_SCB->bufAddr  = (((long)inqdptr) & 0xFFFF) +
					                (((long)inqdptr >> 12) & 0xFFFF0);
				ps2_SCB->bufCount = (long)INQUIRY_BUFFER_SIZE;
				
				regs.h.ah = 0x80;		/* BIOS call to SCSI device */
				regs.h.al = 0x04;		/* Function: Send Device SCB */
				sregs.es  = FP_SEG(ps2_SCB);
				regs.x.bx = FP_OFF(ps2_SCB);
				regs.h.cl = 0x01;		/* SCB type indicator */
				regs.x.dx = SCSI_addr[dev_count].SCSI_key;
				int86x(0x4B, &regs, &regs, &sregs);	/* do it!!! */
//				bios_err(regs.x.ax);
				
				/* check for errors */
				if (regs.x.cflag == 0)
					if (!(strcmp(&inq_data[dev_count][INQ_IDENTIFICATION],
						IDENT_STRING)))
							{
							/* we found a palette so add it to the table */
							SCSI_addr[SCSI_id].Device_Count = dev_count;
							SCSI_addr[SCSI_id].Palette      = YES;
							/* only bump the count if a valid palette */
							SCSI_id++;
							}
				DP_scsi_deallocate(dev_count);
			    } /* end for() */
		    } /* end if() */
		} /* end if(done_once == NO) */
	
return(NOERROR);
}


/*--------------------------------------------------------------------
Need to allocate a SCSI device prior to access.
---------------------------------------------------------------------*/

int DP_scsi_allocate(dev_cnt)
    int	dev_cnt;   
                   
    {
    union REGS regs;

    regs.h.ah	= 0x80;		    /* BIOS call to SCSI device */
    regs.h.al	= 0x02;	        /* Function = Allocate device */
    regs.h.dh	= dev_cnt;      /* open device = next available device */
    regs.h.dl   = 0x02;	        /* Device   = Printer */
    int86(0x4B,&regs,&regs);    /* SCSI interrupt */
//  bios_err(regs.x.ax);
    if (regs.x.cflag != 0)
        return ((int)regs.h.ah);
    else
        {
        SCSI_addr[dev_cnt].SCSI_key = regs.x.dx; /* save the new logical 
			                             device key under the scsi_id */
        return (NOERROR);
        }
    }


/*--------------------------------------------------------------------
 Need to deallocate a SCSI device prior to exiting to DOS.
---------------------------------------------------------------------*/

int DP_scsi_deallocate(dev_cnt)
	int dev_cnt;

    {
    union REGS regs;

    regs.h.ah = 0x80;               /* BIOS call to SCSI device */
    regs.h.al = 0x03;               /* Function: de-allocate device */
    regs.x.dx = SCSI_addr[dev_cnt].SCSI_key; /* deallocate device SCSI_key */
    SCSI_addr[dev_cnt].SCSI_key = (int)NONE;  /* clear the old ID */
    int86(0x4B, &regs, &regs);      /* SCSI Interrupt */
//  bios_err(regs.x.ax);
	
    if (regs.x.cflag != 0)
        return ((int)regs.h.al);
    return (NOERROR);
    }


/*-------------------------------------------------------------------------
 BIOS error checking routine:
     To use this routine for debugging, just uncomment (//) the line of code
     following either an int86() or int86x() SCSI BIOS call.
--------------------------------------------------------------------------*/

int bios_err(err)
    int err;
    {
    int ah;
    int al;

    printf ("AX = 0x%x\n\n",err);
	
    ah = err & 0xff00;
    al = err & 0x00ff;
    switch (ah)
        {
        case    0x0000:
            printf ("AH - No error.\n\n");
            break;
        case    0x0100:
            printf ("AH - Invalid BIOS function or parameter.\n\n");
            break;
        case    0x0200:
            printf ("AH - Device not allocated.\n\n");
            break;
        case    0x0300:
            printf ("AH - Device already allocated.\n\n");
            break;
        case    0x0400:
            printf ("AH - Function not allowed.\n\n");
            break;
        case    0x0500:
            printf ("AH - Adapter unusable, adapter reset required.\n\n");
            break;
        case    0x0600:
            printf ("AH - Request ended in error (see AL).\n");
            break;
        case    0x0700:
            printf ("AH - Adapter reset failed (see AL).\n");
            break;
        case    0x0800:
            printf ("AH - Adapter reconfiguration failed after reset.\n\n");
            break;
        case    0x0900:
            printf ("AH - Adapter time out error.\n\n");
            break;
        case    0x0A00:
            printf ("AH - Adapter in use.\n\n");
            break;
        }
	
    if (al & 0x80)
        printf ("AL - Reset adapter before attempting next command.\n\n");
    if (al & 0x40)
        {
        printf ("AL - Request Command Complete Status or\n"); 
        printf ("     Termination Status Block\n");
        printf ("     to get additional error status data\n\n");
        }
    if (al & 0x20)
        printf ("AL - Reserved\n\n");
    if (al & 0x10)
        printf ("AL - Bits 3 to 0 contain interrupt ID of ISR: %x\n\n",
            al & 0x0F);
	
    return(NOERROR);
    } /* end bios_err() */

