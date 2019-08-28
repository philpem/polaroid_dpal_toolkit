/*  DEC/CMS REPLACEMENT HISTORY, Element DPALETTE.H */
/*  *16    3-MAY-1991 12:05:45 BROGNA "" */
/*  *15    1-APR-1991 18:03:39 BROGNA "Toolkit enhancments" */
/*  *14   25-MAR-1991 16:08:34 BROGNA "" */
/*  *13   18-JAN-1991 04:53:17 BROGNA "MSC 6.0x" */
/*  *12   29-NOV-1990 16:58:04 BROGNA "Version 2.21" */
/*  *11   28-NOV-1990 11:49:31 BROGNA "Dual TimeOut levels" */
/*  *10   16-NOV-1990 12:09:36 BROGNA "Multi-Printer support" */
/*  *9     7-NOV-1990 14:42:14 BROGNA "Added CONFIG_PARAM_BYTES" */
/*  *8     5-NOV-1990 17:37:21 GRABEL "added SCSIID_OFFSET declaration" */
/*  *7    10-OCT-1990 16:57:25 BROGNA "Version 2.1" */
/*  *6     5-OCT-1990 18:44:50 BROGNA "Struct, Buf, and macro name changes" */
/*  *5     3-OCT-1990 15:13:12 BROGNA "Single data structure" */
/*  *4    12-SEP-1990 11:01:39 BROGNA "ToolKit Upgrade" */
/*  *3     6-APR-1990 14:12:45 BROGNA "release 144" */
/*  *2    15-MAR-1990 11:32:53 BROGNA "new toolkit stuff" */
/*  *1    28-FEB-1990 16:09:39 BROGNA "release update" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPALETTE.H */
/* DPALETTE.H
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
 *
 * Include file for applications using the Polaroid Digital Palette Toolkit.
 *
 * This file contains the following:
 *   Constants used to define data structures. (array sizes, etc.)
 *   Typedef DPDATA structure of Digital Palette information.
 *   Constants used as values within data structure.
 *   Data value legality macros.
 *   Data structure translation macros.
 *   Toolkit interface function prototypes.
 *
 * Please refer to the following documents and files for more information:
 *   READ.ME file on Polaroid Digital Palette Toolkit Diskette.
 *   Polaroid Digital Palette Product Specifications.
 *   Polaroid Digital Palette Parallel Port Specifications.
 *   DPALETTE.DOC on Polaroid Digital Palette Toolkit Diskette.
 *   TOOLKIT.DOC on Polaroid Digital Palette Toolkit Diskette.
     
     
29-oct-90  vg   Added DP_SCSIID_OFFSET, Went to version 2.2
29-nov-90  mb   Added Data structure state isolation, Dual Timeout levels, and
                memory model support for small, medium, compact, and large.
                Went to version 2.21
17-jan-91  mb   Went to version 2.3
25-apr-01  mb   Went to version 3.0
 */

#ifndef FILE
#include <stdio.h>
#endif

#define DPALETTE_H
#define DIGITAL_PALETTE "Digital Palette"

/*  This is the string which is placed into the sTKversion array in the
    DP_DATA structure.  The form of the string should remain
    DPalette Driver VX.X ...
    If this convention is adhered to, then anyone can determine the toolkit
    version by parsing a known string to extract the version number.
    For SCSI, the specific SCSI driver which is part of the toolkit is
    strcat'd onto this string.  The entire string cannot exceed the amount
    of room allocated to sTKversion
*/        
#define TOOLKIT_VERSION "DPalette Driver V3.0"

/*
 * Sizes and Limits.
 */
#define CONFIG_PARAM_BYTES    914   /* Size of data in configuration file  */
#define MAX_FILM_FILE_SIZE   1132   /* Maximum size of a film file         */
#define COLOR_TABLE_SIZE      256   /* Size of the color table             */
#define NUM_USER_COLORS         3   /* 0-Red, 1-Grn, 2-Blu                 */
#define MAX_NUM_FILMS          20   /* How many films we can hold          */
#define FILM_NAME_SIZE         24   /* Film name string size IN FILM TABLE */
#define FILM_NAME_LEAD_CHARS    3   /* # of leading chars before film name */
                                    /* The 3 are:
                                          B Black and White
                                          D Downloaded
                                          * ok for camera back
                                     */
#define USER_FILM_NAME_SIZE      (FILM_NAME_SIZE + FILM_NAME_LEAD_CHARS)
#define MAX_CENT_ERROR_MSG_LEN   64   /* max length of error message */
#define STATUS_MSG_SIZE          64
#define OPTIONS_MSG_SIZE         64
#define ASPECT_RATIO_SIZE         3
#define BUFFER_MSG_SIZE           5
#define CAMERA_NAME_SIZE         11
#define DP_COMMAND_SIZE           3
#define DOS_FILE_NAME_SIZE       13
#define NUM_OF_CAMERA_BACKS       4
#define CAMERA_ADJUST_PARAMS      3

/*
 * Union for Toolkit File I/O.
 */
typedef union {
   FILE           *FilePointer ;
	int             FileHandle ;
	} DP_FILE ;
	
/*
 * Data type DP_DATA data structure.
 */
typedef struct {
   char            sDeviceName[24] ;
   short           iPort ;
   short           iHorRes ;
   short           iVerRes ;
   short           iLineLength ;
   short           iHorOff ;
   short           iVerOff ;
   char            cIp ;
   char            cIu ;
   char            cBu ;
   char            cCal ;
   char            cLiteDark ;
   unsigned char   ucaExposTime[NUM_USER_COLORS] ;
   unsigned char   ucaLuminant[NUM_USER_COLORS] ;
   char            caColorBal[NUM_USER_COLORS] ;
   char            caCamAdjust[MAX_NUM_FILMS][CAMERA_ADJUST_PARAMS] ;
   char            saFilmFile[MAX_NUM_FILMS][DOS_FILE_NAME_SIZE] ;
   char            saFilmName[MAX_NUM_FILMS][USER_FILM_NAME_SIZE] ;
   unsigned char   ucaFilmNumber[NUM_OF_CAMERA_BACKS] ;
   short           iVertHeight ;    /* number of lines of exposure data */
   char            cServo ;        /* Servo Mode */
   char            cBkgndMode ;   /* Background Mode */
   unsigned char   ucBkgndValue ;   /* Background Pixel */
   unsigned char   ucaUl_corner_color[NUM_USER_COLORS] ;   /* Upper Left */
   unsigned char   ucaUr_corner_color[NUM_USER_COLORS] ;   /* Upper Right */
   unsigned char   ucaLl_corner_color[NUM_USER_COLORS] ;   /* Lower Left */
   unsigned char   ucaLr_corner_color[NUM_USER_COLORS] ;   /* Lower Right */
   unsigned char   ucaCtTable[NUM_USER_COLORS][COLOR_TABLE_SIZE] ; /* maps */
   char            cImageCompression ; /* Run Length Encoded or Raw */
   unsigned char   ucStatusByte0 ;
   unsigned char   ucStatusByte1 ;
   unsigned int    uiExposedLines ;
   char            sStatusMsg[STATUS_MSG_SIZE] ; /* status ascii bytes */
   unsigned char   ucOptionByte0 ;
   char            sOptionsMsg[OPTIONS_MSG_SIZE] ; /* options ascii */
   char            caAspectRatio[MAX_NUM_FILMS][ASPECT_RATIO_SIZE] ;
   int             iBufferFree ;
   int             iBufferTotal ;
   char            cBufferMsgTerm ;
   char            ucCameraCode ;
   char            sCameraMsg[CAMERA_NAME_SIZE] ; /* camera msg */
   int             iErrorNumber ;            /* error msg byte 0 */
   char            sErrorMsg[MAX_CENT_ERROR_MSG_LEN] ;  /* ascii error */
   int             iErrorClass ;    /* error class */
   char            cDPinitialized ; /* initialized flag */
   char            cDPfound ;       /* found flag */
   char           *cpDataPtr ;    /* Pointer to Data Packet */
   char           *cpMsgPtr ;     /* Pointer to Return Message Packet */
   unsigned int    uiShortTimeOut ;   /* Standard Time out limit */
   unsigned int    uiLongTimeOut ;     /* Time out limit while exposing */
	unsigned int    uiTimeOut ; /* will be one of the above depending... */
   char            cWaitForBuffer ; /* WaitForBuffer flag */
   char            sDPcommand[3] ; /* ASCII Digital Palette Command string */
   int             iNumDataBytes ;  /* number of bytes in data packet */
	int             iFilterColor ; /* Exposure Color */
	char            sTKversion[40] ;
	DP_FILE         unFileIO ;        /* file pointer or handle */
   } DP_DATA ;

/*
	Byte swapping code added for SGI:
*/
#ifdef SGI
#define SSWAP(x) ( ((x) & 0x00ff)<<8 | ((x)>>8 & 0x00ff) )
#else
#define SSWAP(x) (x)
#endif

/*
 * Symbolic color definitions
 */
#define RED       0
#define GREEN     1
#define BLUE      2

/*
 * Symbolic explosure control
 */
#define DARKEST   0
#define DARKER    1
#define DARK      2
#define NORMAL    3
#define LIGHT     4
#define LIGHTER   5
#define LIGHTEST  6

/*
 * Camera Back codes
 */
#define CAMERA_PACK               0
#define CAMERA_35MM               1
#define CAMERA_AUTO               2
#define CAMERA_4X5                3

/*
 * Buffer Warning Suppression
 */
#define SUPPRESS_BUFFER_WARNING  1 /* WaitForBuffer flag is SET */
#define SHOW_BUFFER_WARNING   0 /* WaitForBuffer flag is NOT set */

/*
 * Symbolic definitions for Start Exposure command arguments.
 */
#define SE_NO_PROCESSING         0   /* Don't do any Image Processing */
#define DEFAULT_PROCESSING       SE_NO_PROCESSING

#define SE_ALLOW_MULTIPLE_IMAGES 0   /* Allow multiple images in buffer */
#define SE_ONLY_ONE_IMAGE_IN_BUF 1   /* Only allow one image */
#define DEFAULT_IMAGE_USAGE      SE_ALLOW_MULTIPLE_IMAGES

#define SE_BUF_AS_RECVD          0   /* Execute as soon as recvd */
#define SE_BUF_FULL_COLOR        1   /* Accumulate a full color */
#define SE_BUF_FULL_IMAGE        2   /* Accumulate a full image */
#define SE_BUF_OFF               3   /* Don't use the buffer */
#define DEFAULT_BUFFER_USAGE     SE_BUF_AS_RECVD

#define SE_NORMAL_CALIBRATION    0   /* Auto luma with reasonableness */
#define SE_CAL_NO_CHECK          1   /* Auto luma, no reasonableness */
#define SE_NO_CAL_USE_OLD        2   /* Use prior auto luma */
#define SE_NO_CAL                3   /* Use hardwired autoluma */
#define DEFAULT_CALIBRATION      SE_NORMAL_CALIBRATION

/*
 * Symbolic definitions for Servo command.
 */
#define SERVO_OFF           0 /* Servo is Disabled */
#define SERVO_DELAY         1 /* Delay start, don't change exposure rate */
#define SERVO_STATIC        2 /* Delay start, set static exposure rate */
#define SERVO_DYNAMIC       3 /* Delay start, set dynamic exposure rate*/
#define SERVO_FULL          4 /* Delay start, vary exp. rate with data rate */
#define DEFAULT_SERVO       SERVO_FULL

/*
 * Symbolic definitions for Background Mode command.
 */
#define NO_BACKGROUND        0 /* Background mode disabled */
#define BACKGROUND_LINEAR    1 /* Linear interpolation */

/*
 * Macros
 *
 * Start Exposure parameter legality.
 */
#define SE_PROCESSING_LEGAL(a)     (((a) >= 0) && ((a) <= 0))
#define SE_IMAGE_USAGE_LEGAL(a)    (((a) >= 0) && ((a) <= 1))
#define SE_BUF_USAGE_LEGAL(a)      (((a) >= 0) && ((a) <= 3))
#define SE_CAL_LEGAL(a)            (((a) >= 0) && ((a) <= 3))
/*
 * Servo Mode legality.
 */
#define SERVO_MODE_LEGAL(a)        (((a) >= 0) && ((a) <= 0))
/*
 * Exposure control legality.
 */
#define LIGHT_DARK_LEGAL(a)        (((a) >= 0) && ((a) <= 6))
#define EXP_TIME_LEGAL(a)          (((a) >= 50) && ((a <= 200))
#define LUMINANT_LEGAL(a)          (((a) >= 50) && ((a <= 200))
#define COLOR_BALANCE_LEGAL(a)     (((a) >= 0) && ((a) <= 6))
/*
 * Background mode legality
 */
#define BACKGROUND_MODE_LEGAL(a) (((a) >= 0) && ((a) <= 1))

/*
 * DP_DATA structure Macros
 */
#define DP_FILE_HANDLE(s)             ((s)->unFileIO.FileHandle)
#define DP_FILE_POINTER(s)            ((s)->unFileIO.FilePointer)
#define DP_TOOLKIT_VERSION(s)         ((s)->sTKversion)
#define DP_SHORT_TIMEOUT(s)           ((s)->uiShortTimeOut)
#define DP_LONG_TIMEOUT(s)            ((s)->uiLongTimeOut)
#define DP_STATUS_BYTE_ZERO(s)        ((s)->ucStatusByte0)
#define DP_BUFFERAVAILABLE(s)         (DP_STATUS_BYTE_ZERO(s) & 0x01)
#define DP_EXPOSINGIMAGE(s)           (DP_STATUS_BYTE_ZERO(s) & 0x02)
#define DP_SINGLEIMAGEMODE(s)         (DP_STATUS_BYTE_ZERO(s) & 0x04)
#define DP_STATUS_BYTE_ONE(s)         ((s)->ucStatusByte1)
#define DP_IMAGESINQUEUE(s)           (DP_STATUS_BYTE_ONE(s) & 0x1f)
#define DP_CURRENTCOLOR(s)            ((DP_STATUS_BYTE_ONE(s) & 0x60) >> 5)
#define DP_LINES_EXPOSED(s)           ((s)->uiExposedLines)
#define DP_EXPOSURE_STATUS(s)         ((s)->sStatusMsg)
#define DP_OPTION_BYTE_ZERO(s)        ((s)->ucOptionByte0)
#define DP_MAXHORIZONTALRES(s)        ((DP_OPTION_BYTE_ZERO(s) & 0x01) ? \
                                         4096 : 2048)
#define DP_ON_BOARD_RAM(s)            ((DP_OPTION_BYTE_ZERO(s) & 0x02) ? \
                                         640 : 128)
#define DP_SCSIPORT(s)                (DP_OPTION_BYTE_ZERO(s) & 0x04)
#define DP_ROM_VERSION(s)             ((s)->sOptionsMsg)
#define DP_ASPECT_MESSAGE(s, f)       ((s)->caAspectRatio[f])
#define DP_FREE_BUFFER(s)             ((s)->iBufferFree)
#define DP_TOTAL_BUFFER(s)            ((s)->iBufferTotal)
#define DP_CAMERA_CODE(s)             ((s)->ucCameraCode)
#define DP_CAMERABACK(s)              (DP_CAMERA_CODE(s) & 0x7f)
#define DP_CAMERADESCRIPTION(s)       ((s)->sCameraMsg)
#define DP_AUTOADVANCE(s)             (((DP_CAMERABACK(s) == CAMERA_AUTO) || \
                                         (DP_CAMERABACK(s) == CAMERA_35MM)))
#define DP_MANUALEJECT(s)             (((DP_CAMERABACK(s) == CAMERA_PACK) || \
                                         (DP_CAMERABACK(s) == CAMERA_4X5)))
#define DP_ERRORNUMBER(s)             ((s)->iErrorNumber)
#define DP_ERRORMESSAGE(s)            ((s)->sErrorMsg)
#define DP_ERRORCLASS(s)              ((s)->iErrorClass)
#define DP_DPALETTE_INITIALIZED(s)    ((s)->cDPinitialized)
#define DP_DPALETTE_ONLINE(s)         ((s)->cDPfound)
#define DP_CONFIGERROR(s)             (DP_ERRORCLASS(s) == CONFIGERR)
#define DP_PORTERROR(s)               (((DP_ERRORCLASS(s) <= TIMEOUT) && \
                                         (DP_ERRORCLASS(s) > CONFIGERR)))
#define DP_PRINTERERROR(s)            (((DP_ERRORCLASS(s) > TIMEOUT) && \
                                         (DP_ERRORCLASS(s) < NOERROR)))
#define DP_WARNING(s)                 ((DP_ERRORCLASS(s) > NOERROR))
#define DP_ERROR(s)                   ((DP_ERRORCLASS(s) < NOERROR))
#define DP_ALL_OKAY(s)                ((DP_ERRORCLASS(s) == NOERROR))
#define DP_DEVICE_NAME(s)             ((s)->sDeviceName)
#define DP_PORT(s)                    ((s)->iPort)
#define DP_HOR_RES(s)                 ((s)->iHorRes)
#define DP_VER_RES(s)                 ((s)->iVerRes)
#define DP_LINE_LENGTH(s)             ((s)->iLineLength)
#define DP_HOR_OFFSET(s)              ((s)->iHorOff)
#define DP_VER_OFFSET(s)              ((s)->iVerOff)
#define DP_EXP_IMAGE_PROCESSING(s)    ((s)->cIp)
#define DP_EXP_IMAGE_USAGE(s)         ((s)->cIu)
#define DP_EXP_BUFFER_USAGE(s)        ((s)->cBu)
#define DP_EXP_CALIBRATION(s)         ((s)->cCal)
#define DP_FILTER_COLOR(s)            ((s)->iFilterColor)
#define DP_LIGHTEN_DARKEN(s)          ((s)->cLiteDark)
#define DP_EXPOSURE_TIME(s, c)        ((s)->ucaExposTime[c])
#define DP_RED_EXP_TIME(s)            (DP_EXPOSURE_TIME(s, 0))
#define DP_GREEN_EXP_TIME(s)          (DP_EXPOSURE_TIME(s, 1))
#define DP_BLUE_EXP_TIME(s)           (DP_EXPOSURE_TIME(s, 2))
#define DP_LUMINANT(s, c)             ((s)->ucaLuminant[c])
#define DP_RED_LUMINANT(s)            (DP_LUMINANT(s, 0))
#define DP_GREEN_LUMINANT(s)          (DP_LUMINANT(s, 1))
#define DP_BLUE_LUMINANT(s)           (DP_LUMINANT(s, 2))
#define DP_COLOR_BALANCE(s, c)        ((s)->caColorBal[c])
#define DP_RED_COLOR_BALANCE(s)       (DP_COLOR_BALANCE(s, 0))
#define DP_GREEN_COLOR_BALANCE(s)     (DP_COLOR_BALANCE(s, 1))
#define DP_BLUE_COLOR_BALANCE(s)      (DP_COLOR_BALANCE(s, 2))
#define DP_CAMERA_ADJUST(s, f, d)     ((s)->caCamAdjust[f][d])
#define DP_CAMERA_ADJUST_X(s, f)      (DP_CAMERA_ADJUST(s, f, 0))
#define DP_CAMERA_ADJUST_Y(s, f)      (DP_CAMERA_ADJUST(s, f, 1))
#define DP_CAMERA_ADJUST_Z(s, f)      (DP_CAMERA_ADJUST(s, f, 2))
#define DP_FILM_TABLE_FILE(s, f)      ((s)->saFilmFile[f])
#define DP_FILM_NAME(s, f)            ((s)->saFilmName[f])
#define DP_FILM_PRESENT(s, f)         ((DP_FILM_NAME((s),(f))[0] != '\0'))
#define DP_DEFAULT_FILM(s, c)         ((s)->ucaFilmNumber[c])
#define DP_VER_HEIGHT(s)              ((s)->iVertHeight)
#define DP_SERVO_MODE(s)              ((s)->cServo)
#define DP_BACKGROUND_MODE(s)         ((s)->cBkgndMode)
#define DP_BACKGROUND_PIX_VALUE(s)    ((s)->ucBkgndValue)
#define DP_UPPER_LEFT_COLOR(s, c)     ((s)->ucaUl_corner_color[c])
#define DP_UPPER_RIGHT_COLOR(s, c)    ((s)->ucaUr_corner_color[c])
#define DP_LOWER_LEFT_COLOR(s, c)     ((s)->ucaLl_corner_color[c])
#define DP_LOWER_RIGHT_COLOR(s, c)    ((s)->ucaLr_corner_color[c])
#define DP_COLOR_TABLE(s, c, l)       ((s)->ucaCtTable[c][l])
#define DP_RED_COLOR_TABLE(s, l)      (DP_COLOR_TABLE(s, 0, l))
#define DP_GREEN_COLOR_TABLE(s, l)    (DP_COLOR_TABLE(s, 1, l))
#define DP_BLUE_COLOR_TABLE(s, l)     (DP_COLOR_TABLE(s, 2, l))
#define DP_WAITFORBUFFER(s)           ((s)->cWaitForBuffer)
#define DP_IMAGE_COMPRESSION(s)       ((s)->cImageCompression)
#define DP_ASPECT_RATIO(s, f, XorY)   (DP_FILM_PRESENT((s),(f)) ? \
                                         ((s)->caAspectRatio[f][XorY]) : 1)
#define DP_X_ASPECT(s, f)             (DP_ASPECT_RATIO(s, f, 0))
#define DP_Y_ASPECT(s, f)             (DP_ASPECT_RATIO(s, f, 1))
#define DP_FILMNUMBER(s)              ((s)->ucaFilmNumber[(DP_CAMERABACK(s))])
#define DP_BLACKANDWHITEFILM(s, f)    (DP_FILM_PRESENT((s),(f))      && \
                                         ((s)->saFilmName[f][0] == 'B'))
#define DP_FILMWASDOWNLOADED(s, f)    (DP_FILM_PRESENT((s),(f))      && \
                                         ((s)->saFilmName[f][1] == 'D'))
#define DP_CAMERABACKFILM(s, f)       (DP_FILM_PRESENT((s),(f))      && \
                                         ((s)->saFilmName[f][2] == '*'))

/*
 * Recommended Timeout limits for long and short timeouts.  DP_InitPrinter()
 * sets these values.  Timeout limits other than these values may be set
 * after a successfull return from DP_InitPrinter().
 */
#define STANDARD_TIMEOUT   0x0444 /* DP_SHORT_TIMEOUT(s) = 1 minute */
#define LONG_TIMEOUT       0x2aa8 /* DP_LONG_TIMEOUT(s) = 10 minutes */

/*
 * Errors and Warnings returned from Toolkit routines.
 * These values are always current in DP_DATA error class.
 */
#define NOERROR          0
#define LOGICERR        -1
#define HARDWERR        -2
#define TIMEOUT         -3
#define DISCONNECTED    -4
#define UNKNOWNPORT     -5
#define CONFIGERR       -6
#define DPTOOLKITERR    -7
#define BUFFERWARNING    1
#define EXPOSUREACTIVE   2
#define REMOVEFILM       3

/*
 * CFG_ERROR error numbers
 */
#define CFG_NO_ENVIRONMENT    1
#define CFG_FILE_OPEN_ERROR   2
#define CFG_FILE_READ_ERROR   3
#define CFG_PORT_ERROR        4
#define CFG_FILM_OPEN_ERROR   5

/*
 * case DPTOOLKITERR error numbers
 */
#define TK_BAD_IMG_COMP       1
#define TK_BAD_TERM_METHOD    2

/*
 * GetPrinterStatus() inquiry flags to indicate which inquries to make of the 
 *       Digital Palette.  They can be OR'd together for multiple inquiries.
 */
#define INQ_ERROR       0x0001
#define INQ_CAMERA      0x0002
#define INQ_OPTIONS     0x0004
#define INQ_STATUS      0x0008
#define INQ_FILMS       0x0010
#define INQ_ASPECT      0x0020
#define INQ_BUFFER      0x0040
#define ALL_INQUIRIES   0x007f

/*
 * Image compression.
 */
#define NON_RLE_SEND_AS_RLE       0 /* default; compress before sending */
#define NON_RLE                   1 /* uncompressed, send uncompressed */
#define RLE                       2 /* compressed, send compressed */
#define COMPRESSION_LEGAL(a)      (((a) >= 0) && ((a) <= 2))

/*
 * Image Termination
 */
#define END_OF_EXPOSURE           0 /* normal exposure termination        */
#define ABORT_EXPOSURE            1 /* abort all exposures in queue       */
#define SMART_ABORT               2 /* terminate latest image, save queue */
#define TERM_METHOD_LEGAL(a)      (((a) >= 0) && ((a) <= 2))


/*  DP_SCSIID_OFFSET is used to determine whether we're connected to the
    Palette via a Centronics or SCSI interface.  The DP_PORT number is
    a Parallel Port (Centronics) if it is between 0 and DP_SCSIID_OFFSET - 1.
    If the DP_PORT number is set to a value from DP_SCSIID_OFFSET or
    above, then a connection is made via the SCSI
*/
#define DP_SCSIID_OFFSET  0x10

/*
 * Toolkit function prototypes.
 */
#ifndef DPTKUTIL_C
extern char       * DP_GetConfigPath(void) ;
extern void         DP_Sleep(int) ;
extern int          DP_FileOpen(DP_DATA *, char *) ;
extern int          DP_FileRead(DP_DATA *, char *, int) ;
extern int          DP_FileClose(DP_DATA *) ;
extern unsigned int DP_RLEncode(char *, char *, int) ;
extern void         DP_PathParse(char *, char *, char *, char *, char *) ;
extern void	    DP_SwapShorts(DP_DATA *) ;
#endif

#ifndef DPINIT_C
extern int DP_InitPrinter(DP_DATA *, int, int) ;
extern int DP_DownLoadFilms(DP_DATA *, char *, int) ;
#endif

#ifndef DPSNDPRM_C
extern int DP_StartExposure(DP_DATA *) ;
extern int DP_SendPrinterParams(DP_DATA *) ;
extern int DP_TerminateExposure(DP_DATA *, int) ;
#endif

#ifndef DPINQUIR_C
extern int DP_GetPrinterStatus(DP_DATA *, int) ;
#endif

#ifndef DPSNDIMG_C
extern int DP_SendImageData(DP_DATA *, int, char *,int,int) ;
#endif

#ifndef PATHPARS_C
extern void PathParse(char *, char *, char *, char *, char *) ;
#endif

#ifndef RLENCODE_C
extern unsigned int RLEncode(char *, char *, int) ;
#endif

/* polatime.asm
 */
extern void WaitForTicks(unsigned int) ;
