/*  DEC/CMS REPLACEMENT HISTORY, Element ASPIDRVR.C */
/*  *2     8-AUG-1991 14:56:57 BIELEFELD "updating" */
/*  *1    20-JUN-1991 16:27:01 BIELEFELD "adding a new aspi SCSI driver to the toolkit" */
/*  DEC/CMS REPLACEMENT HISTORY, Element ASPIDRVR.C */
/*
 File aspidrvr.c 
	 Here are all the ASPI low level calls.

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

int  aspi_cmd(unsigned char, unsigned char, 
              unsigned char, char *, unsigned char, char far *, 
              unsigned long, unsigned char, char *, unsigned char, 
              char *, char *) ;
int  aspi_exec(void far *) ;
int  aspi_shell() ;
int  aspi_start() ;
int  aspi_dev_type(int, int, int, int *) ;
int  aspi_host_adapter_inquiry(int, int *, int *, char *, char *, 
                               asrb_ha_unique *) ;
int  aspi_error(char *, int *, int *, int *);

/* Constants */

#define NO	0
#define YES 1
#define INACTIVE 0
#define ACTIVE 1
#define CHECK_CONDITION 2

/* Statics */

static char aspi_name[] = "SCSIMGR$";
static char far *aname = aspi_name;
static void (far *aspi_entry) (asrb_hdr far *) = NULL;
static char far *fptr = (char far *) &aspi_entry;

static union REGS regs;
static struct SREGS segs;
static int handle;



/* Execute the SCSI cmd specified by the caller. Creates an ASRB for
   the request and executes it immediately. Returns -1 if no ASPI shell
   installed, else returns hdr.status value (aspi_status). Returns 
   target_status in status field if ptr not NULL. */

int aspi_cmd (ha, target_id, lun, cmd, cmdlen, bfr, bfrlen, dir, 
extsen, senlen, ha_status, trgt_status)
    unsigned char     ha;
    unsigned char     target_id;
    unsigned char     lun;
    char              *cmd;
    unsigned char     cmdlen;
    char far          *bfr;
    unsigned long     bfrlen;
    unsigned char     dir;
    char              *extsen;
    unsigned char     senlen;
    char              *ha_status;
    char              *trgt_status;

	
    {
    int i, aspi_status;
    struct asrb_initiator_io rb;
    char far *rbb;
	
    rb.hdr.ha_num = ha;                    /* load adapter number */
    rb.hdr.flags  = ASPIF_NO_QUEUE;        /* set the queueing code */
    if ((bfr != NULL) && (bfrlen > 0))     /* if we have data phase */
        rb.hdr.flags |= (dir == 0) ?       /* READ/WRITE data phase */
            ASPIF_INIT_2_TARG_DATA : ASPIF_TARG_2_INIT_DATA;
    rb.hdr.reserved.l = 0;
    rb.target_id = target_id;              /* load scsi_id */
    rb.lun = lun;                          /* load the LUN number */
    rb.cdb_len = cmdlen;                   /* load CDB length */
    for (i=0; (i < cmdlen) && (i < 12); i++)
        rb.cdb[i] = cmd[i];                /* load the CDB bytes */
    rb.data_len = bfrlen;                  /* load the data buffer size */
    rb.data_bfr = (char far *)bfr;         /* load data buffer pointer */
    rb.sense_len = senlen;                 /* load ext sense size */
    rb.asrb_link = NULL;                   /* load linkage pointer (NA) */
    rb.post_proc = NULL;                   /* load post return process */
    rb.hdr.command = ASPIC_EXEC_SCSI_CMD;  /* load execute SCSI CMD */
	
    rbb = (char far *)&rb;
	
    /* execute the aspi command */
    aspi_status = aspi_exec(rbb);
	
    if (trgt_status != NULL) 
        *trgt_status = rb.target_status;
    if (ha_status != NULL)
        *ha_status   = rb.ha_status;
    if (rb.target_status == CHECK_CONDITION)
        memcpy (extsen, &rb.cdb[cmdlen], SENSE_SIZE);
        

    return (aspi_status);
	
    } /* end aspi_cmd() */



/* Here is the ASPI entry point. It will return -1 if the ASPI 
   mgr is not installed otherwise returns aspi_status. */

int aspi_exec(asrb)
    char far *asrb;

	 
    {
    int status, i;
	
    if ((status = aspi_start()) != NOERROR) /* The aspi mgr installed? */
		{ 
      printf("ASPI_MGR NOT FOUND!!!\n");   /* No, it's not... */
      return (status);
      }
		
    (*aspi_entry) ((asrb_hdr far *) asrb);

    while (*(asrb +1) == 0x00) ;
	 
	 
    status = (int) *(asrb +1);

    return(status & 0x00FF);
    } /* end aspi_exec() */


/* aspi shell call (not used) */

int aspi_shell()
    {
    return(1);
    }


/* ASPI startup routine to find the ASPI shell */

int aspi_start()
    {
    if (aspi_entry == NULL)
        {
        /* try to find the ASPI shell */
        /* If found, call it. Else return neg. number */
        regs.h.ah = 0x3D;            /* open a DOS file */
        regs.h.al = 0x00;
        regs.x.dx = FP_OFF(aname);
        segs.ds   = FP_SEG(aname);
        int86x (0x21, &regs, &regs, &segs);
		
        if (regs.x.cflag)
            {
/*          bios_err(regs.x.ax, 0x3D);
*/
            return(-1);              /* Failed to find ASPI shell file */
            }	
	
        handle = regs.x.bx = regs.x.ax;
		
        regs.h.ah = 0x44;            /* DOS IOCTL Read */
        regs.h.al = 0x02;
        regs.x.dx = FP_OFF(fptr);
        segs.ds   = FP_SEG(fptr);
        regs.x.cx = 4;
        int86x (0x21, &regs, &regs, &segs);
        
        if (regs.x.cflag)
            {
/*			bios_err (regs.x.ax,0x44);
*/
            return(-2);
            }
/*		
        printf ("aspi_entry = %4.4x:%4.4x\n", 
         FP_SEG(*aspi_entry) ,FP_OFF(*aspi_entry));
*/		  
        regs.h.ah = 0x3E;            /* DOS close file */
        regs.x.bx = handle;
        int86x (0x21, &regs, &regs, &segs);
		
        if (regs.x.cflag)
            {
 /*         bios_err(regs.x.ax,0x3E);
 */
            return(-3);
            }
        }

    return(NOERROR);
    } /* end aspi_start() */



/* This routine will do an inquiry into the host SCSI adapter to determin
   the number of host adapters installed. Returns hdr.status value. 
       
       If ha_tot    != NULL, returns total number of host adapters.
       If scsi_id   != NULL, returns host adapter SCSI id number.
       If aspi_id   != NULL, returns ASPI id string.
       If ha_id     != NULL, returns host adapter id string.
       If ha_unique != NULL, returns host adapter unique info (16 bytes)
*/

int aspi_host_adapter_inquiry (ha, ha_tot, scsi_id, aspi_id, ha_id, ha_unique)
    int ha;
    int *ha_tot;
    int *scsi_id;
    char *aspi_id;
    char *ha_id;
    asrb_ha_unique *ha_unique;
	
    {
    int aspi_status;
    asrb_ha_inquiry rb;
    static ha_count = 0;
	
    rb.hdr.command    = ASPIC_HA_INQUIRY;
    rb.hdr.ha_num     = ha;
    rb.hdr.reserved.l = 0;
	 rb.hdr.flags      = 0;
	
    if ((aspi_status = aspi_exec (&rb)) != 1) return(aspi_status);
	
    if (rb.hdr.status != ASPIS_INVALID_HA_NUM)
        ha_count = rb.ha_tot;
	
    if (ha_tot != NULL)
        {
        if (rb.hdr.status != ASPIS_INVALID_HA_NUM) 
            *ha_tot = rb.ha_tot;
        else 
            *ha_tot = ha_count;
        }
	  
    if (aspi_status == ASPIS_SCSI_REQ_COMPLETE)
        {
        if (scsi_id != NULL) 
            *scsi_id = rb.scsi_id;
        if (aspi_id != NULL) 
            {
            memcpy (aspi_id, rb.aspi_id, 16);
            aspi_id[16] = '\0';
            }
        if (ha_id   != NULL) 
            {
            memcpy (ha_id, rb.ha_id, 16);
            ha_id[16] = '\0';
            }
        if (ha_unique != NULL) 
            memcpy ((char *)ha_unique, (char *)&rb.ha_unique,
             sizeof(asrb_ha_unique));
        }
	  
    return(rb.hdr.status);
	
    } /* end aspi_host_adapter_inquiry() */



/* Returns hdr.status value. ha/target_id/lun identifies the device for 
   which device_type will be returned. Only returns device_type if 
   device_type != NULL. */

int aspi_dev_type(ha, target_id, lun, device_type)
    int ha;
    int target_id;
    int lun;
    int *device_type;
	
    {
    asrb_get_dev_type rb;
    char far *rbb;
    int aspi_status;
	 
    rb.hdr.command    = ASPIC_GET_DEV_TYPE;
    rb.hdr.ha_num     = ha;
    rb.hdr.reserved.l = 0;
	 rb.hdr.flags      = 0;
    rb.target_id      = target_id;
    rb.lun            = lun;
	
    rbb = (char far *)&rb;
    aspi_status = aspi_exec(rbb);
	
    if ((device_type != NULL) && (aspi_status == 1)) 
        *device_type = (int) rb.device_type;
	
    return (aspi_status);
    } /* end aspi_dev_type() */



int aspi_error(cdb, aspi_err, trgt_err, ha_err)
    char cdb[];
    int *aspi_err;
    int *trgt_err;
    int *ha_err;
    
    {
    if (*aspi_err < 2)
		return(NOERROR);
	
    printf ("SCSI cmd: %2.2x\n", cdb[0]);
    if (cdb[0] == 0x0C)
        printf ("DFR cmd: %2.2x\n", cdb[2]);
    printf ("0x%2.2x ", *aspi_err);
    switch(*aspi_err)
		{
		case	0x00:
			printf("SCSI request in progress.\n");
			break;
		case	0x01:
			printf("SCSI request completed without error.\n");
			break;
		case	0x02:
			printf("SCSI request aborted by host.\n");
			break;
		case	0x03:
			printf("Unable to abort SCSI request.\n");
			break;
		case	0x04:
			printf("SCSI request completed with error.\n");
			break;
		case	0x80:
			printf("Invalid SCSI request.\n");
			break;
		case	0x81:
			printf("Invalid host adapter number.\n");
			break;
		case	0x82:
			printf("SCSI device not installed.\n");
			break;
        default:
            printf("Unknown ASPI_shell error.\n");
		}

    if (*aspi_err > 1)
        {
        printf ("0x%2.2x ", *trgt_err);
        switch (*trgt_err)
            {
            case    0x00:
                printf("No target status.\n");
                break;
            case    0x02:
                printf("Check Condition.\n");
                break;
            case    0x08:
                printf("Specified LUN busy.\n");
                break;
            case    0x18:
                printf("Reservation Conflict.\n");
                break;
            default:
                printf("Unknown target error.\n");
            }
        }
    if (*aspi_err > 1)
        {
        printf ("0x%2.2x ", *ha_err);
        switch (*ha_err)
            {
            case    0x00:
                printf("No host adapter detected error.\n");
                break;
            case    0x11:
                printf("Selection Timeout.\n");
                break;
            case    0x12:
                printf("Date overrun/underrun.\n");
                break;
            case    0x13:
                printf("Unexpected bus free.\n");
                break;
            case    0x14:
                printf("Target bus phase sequence failure.\n");
                break;
            default:
                printf("Unknown adapter error.\n");
            }
        }
	
	return(NOERROR);
    }

int bios_err (err_code, cmd)
int err_code;
int cmd;
    {
    switch (cmd)
        {
        case    0x3D:
        case    0x44:
        case    0x3E:
            switch(err_code)
                {
                case 0x00:
                     printf("Successful BIOS Call.\n");
                     break;
                case 0x01:
                     printf("Invalid function number.\n");
                     break;
                case 0x02:
                     printf("File not found.\n");
                     break;
                case 0x03:
                     printf("Path not found.\n");
                     break;
                case 0x04:
                     printf("No more handles available.\n");
                     break;
                case 0x05:
                     printf("Access denied.\n");
                     break;
                case 0x06:
                     printf("Invalid handle.\n");
                     break;
                case 0x07:
                     printf("Bad memory control blocks.\n");
                     break;
                case 0x08:
                     printf("Insufficient memory.\n");
                     break;
                case 0x09:
                     printf("Invalid memory block address.\n");
                     break;
                case 0x10:
                     printf("Invalid enviroment.\n");
                     break;
                case 0x11:
                     printf("Invalid format.\n");
                     break;
                case 0x12:
                     printf("Invalid access code.\n");
                     break;
                case 0x13:
                     printf("Invalid data.\n");
                     break;
                case 0x14:
                     printf("Not used.\n");
                     break;
                case 0x15:
                     printf("Invalid drive specification.\n");
                     break;
                case 0x16:
                     printf("Removing current directory.\n");
                     break;
                case 0x17:
                     printf("Not same device.\n");
                     break;
                case 0x18:
                     printf("No more files to be found.\n");
                     break;
                case 0x19:
                     printf("Disk is write protected.\n");
                     break;
                case 0x20:
                     printf("Unknown disk.\n");
                     break;
                case 0x21:
                     printf("Drive is not ready.\n");
                     break;
                case 0x22:
                     printf("Unknown command.\n");
                     break;
                case 0x23:
                     printf("Data error (CRC).\n");
                     break;
                case 0x24:
                     printf("Bad request length.\n");
                     break;
                case 0x25:
                     printf("Seek error.\n");
                     break;
                case 0x26:
                     printf("Unknown media type.\n");
                     break;
                case 0x27:
                     printf("Sector not found.\n");
                     break;
                case 0x28:
                     printf("Printer out of paper.\n");
                     break;
                case 0x29:
                     printf("Write fault.\n");
                     break;
                case 0x30:
                     printf("Read fault.\n");
                     break;
                case 0x31:
                     printf("General failure.\n");
                     break;
                default:
                     printf ("Unknown BIOS error.\n");
                }
        default:
        printf ("Unknown BIOS command\n");
        }
    } /* end bios_err */
