/*  DEC/CMS REPLACEMENT HISTORY, Element SCLOWLVL.H */
/*  *2     3-MAY-1991 12:13:47 BROGNA "" */
/*  *1     5-NOV-1990 18:08:59 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCLOWLVL.H */
/*      sclowlvl.h
 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
	 
    Contains declarations which are needed for the low level host specific
    functions of the Digital Palette SCSI Toolkit.
        
    This include file also provides documentation on how to build a new
    set of low level routines from scratch.
        
*/

#define CDB_SIZE 6          /* size of the scsi command description block */
#define CHECK_CONDITION 2   /* check condition bit in the status byte     */

#define SENSE_SIZE 10       /* size of the extended sense data block      */

#define LINK_BIT_IN_CDB 1   /* link bit in the CDB                        */


/* The following values are the 'dir' values for the DP_doscsi_cmd function
   which indicate what happens during the data phase for the scsi command.
   For most host scsi drivers, this information is not required since the 
   target device controls the flow of data to and from the initiator.  Some
   host driver implementations, however, require this info; in particular
   the Mac SCSI driver.  
*/
#define  SCSI_READ    1
#define  SCSI_NODATA  0
#define  SCSI_WRITE  -1

/* the following two values are returned by the low level routines to the
   toolkit.  If the value returned is not one of these values, then the
   host specific error reporting routine, host_retrieve_scsi_error(), is
   called.
*/       
#define SCSI_COMMAND_OK 0   /* all went well, no errors */
#define EXTSENSE_VALID  1   /* a CHECK CONDITION in the status occurred and
                               a GetExtendedSense command was issued.  The
                               contents of the extended sense array are
                               valid.
                            */


