/*  DEC/CMS REPLACEMENT HISTORY, Element TKSCCMDS.H */
/*  *5     3-MAY-1991 12:15:10 BROGNA "" */
/*  *4    20-FEB-1991 08:55:41 CAMPBELL_T "Added sc_inquire_short macro" */
/*  *3    18-JAN-1991 22:48:45 GRABEL "changes to run under LSC on the MAC" */
/*  *2    18-JAN-1991 04:58:22 BROGNA "MSC 6.0x" */
/*  *1     5-NOV-1990 18:13:51 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKSCCMDS.H */
/*  tksccmds.h
 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
	 
    include file which contains macros for all of the Digital Palette/SCSI
    commands.  These macros are used to collapse all of the SCSI commands
    into one of two calls.
        
    All of the include files which are required to define the SCSI commands
    and their parameters are also pulled in by this file.  There is no
    need, therefore, for the C sources to explicitly include these files.

        
21-sep-90  vg       initial.....
 4-oct-90  vg       moved environment declarations here...
18-jan-91  vg       typo in function declaration for SCDPRERR_C
30-jan-91  vg       added sc_inquire_short macro
*/

/*  include all of the files which define the SCSI commands and their
    parameters
*/
#include "tkdfpcmd.h"     /*  definitions of the Palette SCSI commands */

#include "tkmodsel.h"     /* data in the mode select command */
#include "tkmodsen.h"     /* data in the mode sense command */
#include "tkinquir.h"     /* data in the inquire command */
#include "sclowlvl.h"

#define DP_CMD_PACKET_SIZE      5   /* internal buffer overhead */

#define SHORT_INQUIRY_BUFFER_SIZE 32

/*  test unit ready, no arguments are required */
#define sc_test_unit_ready()  \
    DP_doscsi_cmd( dp, TEST_UNIT_READY, (unsigned char *)0, 0, 0, \
                   COLOR_NONE, SCSI_NODATA )

/* inquire, the sole argument is a pointer to where the data will go */
#define sc_inquire( p )  \
    DP_doscsi_cmd( dp, INQUIRY, p, INQUIRY_BUFFER_SIZE, 0, \
                   COLOR_NONE, SCSI_READ)

/* inquire, the sole argument is a pointer to where the data will go */
/* a short inquire command is issued by the init code when all scsi
   id's are being searched for Palettes.  A short inquire will only read
   the inquire data which is required in the SCSI spec.  This generally
   speeds the search up because the low level driver doesn't get into
   a data phase timeout if other scsi devices are connected.
*/
#define sc_short_inquire( p )  \
    DP_doscsi_cmd( dp, INQUIRY, p, SHORT_INQUIRY_BUFFER_SIZE, 0, \
                   COLOR_NONE, SCSI_READ)

/* inquire, the sole argument is a pointer to where the data will go */
#define sc_mode_sense( p )  \
    DP_doscsi_cmd( dp, MODE_SENSE, p, SENSE_BUFFER_SIZE, 0, \
                   COLOR_NONE, SCSI_READ )

/* stop print, no args */
#define sc_stop_print()  \
    DP_doscsi_cmd( dp, STOP_PRINT, (unsigned char *)0, 0, 0, \
                   COLOR_NONE, SCSI_NODATA )

/* mode_select, the sole argument is a pointer to the data */
#define sc_mode_select( p )  \
    DP_doscsi_cmd( dp, MODE_SELECT, p, SELECT_BUFFER_SIZE, 0, \
                   COLOR_NONE, SCSI_WRITE )

/* print, three args are necessary, the data pointer, size, and color */
#define sc_print( p, s, c )  \
    DP_doscsi_cmd( dp, PRINT, p, s, 0, c, SCSI_WRITE )

/**** The following are definitions for the DFR sub-commands ****/

/* start_exposure, no args */
#define sc_start_exposure()  \
    DP_doscsi_cmd( dp, DFRCMD, (unsigned char *)0, 0, START_EXPOSURE, \
                   COLOR_NONE, SCSI_NODATA )
            
/* set color table, two args, a data pointer and color */            
#define sc_set_color_table( c, p ) \
    DP_doscsi_cmd( dp, DFRCMD, p, 256, SET_COLOR_TAB, c, SCSI_WRITE )
            
/* get color table, two args, a data pointer and color */            
#define sc_get_color_table( c, p )  \
    DP_doscsi_cmd( dp, DFRCMD, p, 256, GET_COLOR_TAB, c, SCSI_READ )
            
/* terminate exposure, no args */
#define sc_terminate_exposure() \
    DP_doscsi_cmd( dp, DFRCMD, (unsigned char *)0, 0, TERMINATE_EXPOSURE, \
                   COLOR_NONE, SCSI_NODATA )


/* film name, two args: film_no and data pointer */
#define sc_film_name( f, p ) \
    DP_scsi_read_forfilm( dp, DFRCMD, p, FILM_NAME_SIZE, FILM_NAME, f )
            
/* film aspect ratio, two args: film_no and data pointer */
#define sc_film_aspect( f, p ) \
    DP_scsi_read_forfilm( dp, DFRCMD, p, 2, ASPECT_RATIO, f )

/* current status, the sole arg is a data pointer */
#define sc_curr_status( p ) \
    DP_doscsi_cmd( dp, DFRCMD, p, 7, CURRENT_STATUS, COLOR_NONE, SCSI_READ )

/* reset to default, no args */
#define sc_reset_to_default() \
    DP_doscsi_cmd( dp, DFRCMD, (unsigned char *) 0, 0, RESET_TO_DFLT, \
                   COLOR_NONE, SCSI_NODATA )

/* background sweeps, the sole arg is a data pointer */
#define sc_sweep( p )  \
    DP_doscsi_cmd( dp, DFRCMD, p, SWEEP_BUF_SIZE, SWEEPS, \
                   COLOR_NONE, SCSI_WRITE )
            
/* camera adjust, the sole arg is a data pointer to four chars */
#define sc_camera_adjust( p )  \
    DP_doscsi_cmd( dp, DFRCMD, p, CABUF_SIZE, CAMERA_ADJUST, \
                   COLOR_NONE, SCSI_WRITE )

/* film download, two args: data pointer and data size */
#define sc_film_download( p, s ) \
    DP_doscsi_cmd( dp, DFRCMD, p, s, FILMTBL_LOAD, COLOR_NONE, SCSI_WRITE )


/* function prototypes */
extern int DP_scsi_driver(char *, int, char *, int, char *, int) ;
extern int DP_scsi_setup(DP_DATA *, char **) ;
extern int DP_host_retrieve_scsi_error(DP_DATA *, int) ;

#ifndef SCDPSCMD_C
extern int DP_scsi_init(DP_DATA *) ;
extern int DP_doscsi_cmd(DP_DATA *, int, unsigned char *, int, int, int, int);
extern int DP_scsi_read_forfilm(DP_DATA *, int, char *, int, int, int) ;
extern int DP_firmware_rev(DP_DATA *) ;
#endif

#ifndef DPINQUIR_C
extern int DP_ConfirmBuffer(DP_DATA *, int) ;
extern int DP_ExposureWarning(DP_DATA *) ;
extern void DP_sleep(int) ;
#endif

#ifndef SCDPRERR_C
extern int DP_retrieve_scsi_error(DP_DATA *, int, char *) ;
#endif
