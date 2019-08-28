/*  DEC/CMS REPLACEMENT HISTORY, Element ASPI.H */
/*  *1     8-AUG-1991 14:54:20 BIELEFELD "ADDING ASPI.H TO CMS" */
/*  DEC/CMS REPLACEMENT HISTORY, Element ASPI.H */
/*
	ASPI.H
	
	Defines ASPI structures, command and return codes, etc.

	These are further defined in the "ASPI, Advanced SCSI 
	Programmer's Interface, General Specifications, and MS-DOS
	Access Method" document published by Trantor.
*/


/* ASPI Request Block (ASRB) Structure. */

#pragma pack(1)

typedef struct {

	unsigned char command;
	unsigned char status;
	unsigned char ha_num;
	unsigned char flags;
	union {
		unsigned char c [4];
		unsigned long l;
	} reserved;

} asrb_hdr;


/* Command specific parameters. */

/* Host Adapter Inquiry (Command 0h) */

typedef struct {

	unsigned char card_type;
	unsigned base_port;
	unsigned char dma_allowed;
	unsigned char dma_channel;
	unsigned char int_allowed;
	unsigned char int_channel;
	unsigned char resv[9];

} asrb_ha_unique;

typedef struct {

	asrb_hdr hdr;
	unsigned char ha_tot;
	unsigned char scsi_id;
	unsigned char aspi_id [16];
	unsigned char ha_id [16];
	asrb_ha_unique ha_unique;
	
} asrb_ha_inquiry;


/* Get Device Type (Command 1h) */

typedef struct {

	asrb_hdr hdr;
	unsigned char target_id;
	unsigned char lun;
	unsigned char device_type;
	unsigned char resv [6];
	
} asrb_get_dev_type;


/* Execute SCSI Command (Command 2h) */

/* Note: sense_data [] field normally exists at end of structure.
However, cdb [] is variable size, so functions must calculate the
sense_data [] location as if it were part of the cdb [] field.
sense_data [] begins at cdb [cdb_len]. */

/* Note: When allocating memory for this structure, be sure to add
the maximum sense_data [] size (sense_len) to the structure size
returned by sizeof (asrb_initiator_io) operator. */

typedef struct asrb_initiator_io {

	asrb_hdr hdr;
	unsigned char target_id;
	unsigned char lun;
	unsigned long data_len;
	unsigned char sense_len;
	unsigned char far *data_bfr;
	struct asrb_initiator_io far *asrb_link;
	unsigned char cdb_len;
	unsigned char ha_status;
	unsigned char target_status;
	void (far *post_proc) (struct asrb_initiator_io far *);
	unsigned char os_resv [12];
	unsigned char aspi_resv [22];
	unsigned char cdb [16];

} asrb_initiator_io;


/* Abort SCSI Command (Command 3h) */

typedef struct {

	asrb_hdr hdr;
	asrb_initiator_io far *asrb;
	
} asrb_initiator_abort;


/* Reset SCSI Device (Command 4h) */

typedef struct {

	asrb_hdr hdr;
	unsigned char target_id;
	unsigned char lun;
	unsigned char resv [14];
	unsigned char ha_status;
	unsigned char target_status;
	unsigned char resv1 [38];
	
} asrb_reset_device;


/* Set Host Adapter Parameters (Command 5h) */

typedef struct {

	asrb_hdr hdr;
	asrb_ha_unique ha_unique;
	
} asrb_set_ha_parms;


/* Establish Target Node (Command 40h) */
/* Transfer Target Data (Command 42h) */

typedef struct {

	asrb_hdr hdr;
	unsigned char target_id;
	unsigned char lun;
	unsigned char init_id;
	void (far *cmd_len_proc) (struct asrb_target_io far *);
	unsigned char cdb_len;
	unsigned char cdb [12];
	void (far *exec_proc) (struct asrb_target_io far *);
	unsigned char next_phase;
	unsigned long data_len;
	unsigned char far *data_bfr;
	unsigned char status;
	void (far *msg_proc) (struct asrb_target_io far *);
	unsigned char send_msg;
	unsigned char recv_msg;
	unsigned char bus_status;
	void (far *reset_proc) (struct asrb_target_io far *);
	unsigned char os_resv [12];
	unsigned char aspi_resv [22];

} asrb_target_io;


/* Abort Target Node (Command 41h) */

typedef struct {

	asrb_hdr hdr;
	asrb_target_io far *asrb;
	
} asrb_target_abort;


/* Reset SCSI Bus (Command 80h) */

typedef struct {

	asrb_hdr hdr;
	
} asrb_reset_scsi_bus;

#pragma pack()

/* Command (hdr.command) codes */

#define ASPIC_HA_INQUIRY 0x0
#define ASPIC_GET_DEV_TYPE 0x1
#define ASPIC_EXEC_SCSI_CMD 0x2
#define ASPIC_ABORT_SCSI_CMD 0x3
#define ASPIC_RESET_SCSI_DEV 0x4
#define ASPIC_SET_HA_PARMS 0x5

#define ASPIC_EST_TARGET_NODE 0x40
#define ASPIC_ABORT_TARGET_NODE 0x41
#define ASPIC_XFER_TARGET_DATA 0x42

#define ASPIC_RESET_SCSI_BUS 0x80


/* Status (hdr.status) codes */

#define ASPIS_SCSI_REQ_IN_PROGRESS 0x0
#define ASPIS_SCSI_REQ_COMPLETE 0x1
#define ASPIS_SCSI_REQ_ABORTED 0x2
#define ASPIS_SCSI_REQ_NOT_ABORTED 0x3
#define ASPIS_SCSI_REQ_COMPLETE_ERR 0x4

#define ASPIS_INVALID_SCSI_REQ 0x80
#define ASPIS_INVALID_HA_NUM 0x81
#define ASPIS_NO_SCSI_DEV 0x82


/* hdr.flags for Execute SCSI Command */

#define ASPIF_NO_DATA 0x0
#define ASPIF_TARG_2_INIT_DATA 0x8
#define ASPIF_INIT_2_TARG_DATA 0x10

#define ASPIF_NO_QUEUE 0x0
#define ASPIF_QUEUE 0x1


/* ha_status values */

#define ASPIHA_OK 0x0
#define ASPIHA_NO_SEL 0x11
#define ASPIHA_DATA_OVERRUN 0x12		/* Also, data underrun */
#define ASPIHA_BUS_FREE 0x13
#define ASPIHA_WRONG_PHASE 0x14


/* target_status values (partial list) */

#define ASPITARG_OK 0x0
#define ASPITARG_CHECK 0x2
#define ASPITARG_BUSY 0x8
#define ASPITARG_RESERVED 0x18


/* Target Mode next_phase values */

#define ASPINEXT_BUS_FREE 0x0
#define ASPINEXT_MSG_RECV 0x1
#define ASPINEXT_MSG_SEND 0x2
#define ASPINEXT_CMD 0x3
#define ASPINEXT_DATA_RECV 0x4
#define ASPINEXT_DATA_SEND 0x5
#define ASPINEXT_STATUS 0x6
#define ASPINEXT_DISCONNECT 0x7


/* Target Mode bus_status values */

#define ASPIBUS_FREE 0x0
#define ASPIBUS_CONNECTED 0x1
#define ASPIBUS_DISCONNECTED 0x2
#define ASPIBUS_RESET 0x80


/* End of file. */

