/*  DEC/CMS REPLACEMENT HISTORY, Element TKDFPCMD.H */
/*  *2     3-MAY-1991 12:14:33 BROGNA "" */
/*  *1     5-NOV-1990 18:12:56 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKDFPCMD.H */
/*      tkdfpcmd.h

    Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation

    Palette SCSI command definitions
        
29-jun-90  vg       initial....
 3-aug-90  vg       added low level definitions

*/


/* color definitions */
#define COLOR_RED   0
#define COLOR_GREEN 1
#define COLOR_BLUE  2
#define COLOR_NONE  0


/* Main SCSI commands */
#define TEST_UNIT_READY     0x00
#define REQUEST_SENSE       0x03
#define PRINT               0x0A
#define DFRCMD              0x0C
#define FLUSH_BUFFER        0x10
#define INQUIRY             0x12
#define MODE_SELECT         0x15
#define MODE_SENSE          0x1A
#define STOP_PRINT          0x1B


/* DFR subcommand numbers */
#define START_EXPOSURE      0
#define SET_COLOR_TAB       1
#define GET_COLOR_TAB       2
#define TERMINATE_EXPOSURE  3
#define FILM_NAME           4
#define ASPECT_RATIO        5
#define CURRENT_STATUS      6
#define RESET_TO_DFLT       7
#define SWEEPS              8
#define CAMERA_ADJUST       9
#define FILMTBL_LOAD       10

#define SWEEP_BUF_SIZE     14
#define CABUF_SIZE          4

