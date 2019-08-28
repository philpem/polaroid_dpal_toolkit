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
   int             iPort ;
   int             iHorRes ;
   int             iVerRes ;
   int             iLineLength ;
   int             iHorOff ;
   int             iVerOff ;
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
   int             iVertHeight ;    /* number of lines of exposure data */
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
#define DP_SCSIPORT(s)                (DP_OPTION_BYTE_ZERO(s) & 0x0 0x04)
#definROt-S_BYTLha)      B04)
#definde cMleg & 0x, f0x0 0x04finROts ascii */
  f]      ((s)->unF
#d(s)->uc& 0x0 0x04)
#definROtIO_SIZE] ;     ((s)->unFilonB(s)->uc& 0x0 0x04)
#dfinROtIO_SIZEe ;
  (DP_STATUS_BYAUTO  C 0x02)
#definefine DP_OPTIsgTerm ;
  (DP_STATUS_BYAUTO nd m640 : 128)
#defi(S_BYAUTO  C 0x02)    7   (DP_STATUS_BYAUTO DESCRICSIPO640 : 128)finROt-eraCode ;     B04)
#def   ADV((a 02)
#definefine D(S_BYAUTO nd m640 ==A_35MM      ) ||P_OPTION_BYTE_ZERO(s) & 0x02) ? \
          (S_BYAUTO nd m640 ==A_35MM      )     (((a) > ((s)NUALEJde 02)
#definefine D(S_BYAUTO nd m640 ==A_35MM  ack ) ||P_OPTION_BYTE_ZERO(s) & 0x02) ? \
          (S_BYAUTO nd m640 ==A_35MM     )     (((a) > ((     r  Buc& 0x0 0x04)
#definROtIra msg */
    (((a) > ((     Mleg & 0x0x0 0x04)
#dfinROt byte 0 *    (((a) > ((     CLASnes)
#define
#definROtIra msr */    (((a) > ((DPALETTer *ITILT_Z) >> 5)
finROts class */
      (((a) > ((DPALETTermagI0x04)
#define DP_STed flag   (DP_STATUS_BYONFIG     & 0x0 0x04)
#def ((     CLASnes)==A_ONFIG     (DP_STATUS_B         & 0x0 0x04)
#dene D(S_B     CLASnes)E_LShortTiGROUN_OPTION_BYTE_ZERO(s) & 0x02) ? \
          (S_B     CLASnes)>A_ONFIG         (((a) > ((PRFileH     & 0x0 0x04)
#d D(S_B     CLASnes)>LShortTiGROUN_OPTION_BYTE_ZERO(s) & 0x02) ? \
          (S_B     CLASnes)<und           (((a) > ((is SET 640 : 128)
#define D(S_B     CLASnes)>Lnd          (((a) > ((     640 : 128)
#define e D(S_B     CLASnes)<und           B04)
#defullOKAY640 : 128)
#define f ((     CLASnes)==And           B04)
#deDEVIa <r   & 0x0 0x04)
#definROt-DeviceNraC  (DP_STATUS_B    640 : 128)
#define e efinROtIPTIM  (DP_STATUS_B>uc_yte0)
#defineefine e efinROtIHorRs)           ((sERO_yte0)
#defineefine e efinROtIVerRs)           ((sgI0x   sGTH& 0x0 0x04)
#definROtIED(s    cI  (DP_STATUS_B>uc_o cNG es)
#define
#definROtIHorOf   (DP_STATUS_BERO_o cNG es)
#define
#definROtIVerOf   (DP_STATUS_B >=  0) &&ter legali>> 5)
finROtsIp  (DP_STATUS_B >=  0) &&Ug & 0x0x0 0x04)finROtsIu  (DP_STATUS_B >= the buffer *g)
#define DP_OPfer  (DP_STATUS_B >= ULT_CALIBRA0x0x0 0x04)finROtsC
  (DP_STATUS_BFilleH_(s) & 0x1f)
#define DnROtIa packet */           ((sgI#defNl legE & 0x0 0x04)
finROtsLiteDark           ((s)->uiExpoShor0x, c
#define DP_OPTIaS_EXPEOUT[c]      ((s)->unolo_ >= 0) & 0x1f)
#define ((s)->uiExpoShor0x,       (((a) >((s
#de_ >= 0) & 0x1f)
#defi ((s)->uiExpoShor0x, L(a)    (((a) ((s
#d_ >= 0) & 0x1f)
#defin ((s)->uiExpoShor0x, 2(a)    (((a) ((= 50) &&0x, c
#defineefine DP_OPTIaLuminant[c]      ((s)->unolo_= 50) &&0xx1f)
#define ((s= 50) &&0x,       (((a) >((s
#de_= 50) &&0xx1f)
#defi ((s= 50) &&0x, L(a)    (((a) ((s
#d_= 50) &&0xx1f)
#defin ((s= 50) &&0x, 2(a)    (((a) ((= 50) && ((a 0x, c
#define DP_OPcaet */Bal[c]      ((s)->unolo_= 50) && ((a 0x0 : 128)f ((= 50) && ((a 0x,       (((a) >((s
#de_= 50) && ((a 0x0 : 12f ((= 50) && ((a 0x, L(a)    (((a) ((s
#d_= 50) && ((a 0x0 : 128f ((= 50) && ((a 0x, 2(a)    (((a) ((=35MM   DJUS&0x, f_DE
#definP_OPcaeamAdjust f][d]      ((s)->un=35MM   DJUS&_X0x, f0x0 0x0(S_BYAUTO   DJUS&0x, f_D      (((a) >((s=35MM   DJUS&_Y0x, f0x0 0x0(S_BYAUTO   DJUS&0x, f_DL(a)    (((a) ((=35MM   DJUS&_Z0x, f0x0 0x0(S_BYAUTO   DJUS&0x, f_D2(a)    (((a) ((FilM_TsBytnFile0x, f0x0 0x0(inROt-aa pmR(s) f]      ((s)->unFilM_r   & , f0x0 0x040 0x0(inROt-aa pmNraC f]      ((s)->unFilM_g SuE&&0x, f0x0 0x040  f ((FilM_r   &inR,(f))[
	!= '\0'      B04)
#deDEe withFilM0x, c
#definee DP_OPTIaa pmN */
[c]      ((s)->unERO_HEI#dees)
#define
#definROtIVertHeigh)            ((s Mode leges)
#define
#definROtc) >= a)    (((a) ((sd mode legalit0x0x0 0x04)finROtsBkgndM;
  (DP_STATUS_Bsd mode legPIX_VALsByte1)
 DP_OPTIBkgndValu
  (DP_STATUS_BingRO_LEFT_(s) & 0, c
#def DP_OPTIaUl_corner_     [c]      ((s)->uningRO_Rntrol(s) & 0, c
#de DP_OPTIaUr_corner_     [c]      ((s)->un DERO_LEFT_(s) & 0, c
#def DP_OPTIaLl_corner_     [c]      ((s)->un DERO_Rntrol(s) & 0, c
#de DP_OPTIaLr_corner_     [c]      ((s)->un= 50) TsByte0, c, l0 : 128)finROtTIaCtT    [c][l]      ((s)->unolo_= 50) TsByte0, l0 : 128f ((= 50) TsByte0, 0, l0    (((a) >((s
#de_= 50) TsByte0, l0 : 1f ((= 50) TsByte0, 1, l0    (((a) >((ss
#d_= 50) TsByte0, l0 : 12f ((= 50) TsByte0, 2, l0    (((a) >((sWAITFORs)->uc& 0x0 0x04)
#finROtsWaitF*/B_SIZE(s)         ((s)->u(= Mg Su(s) & 0x0 0x04finROtsI    Compr 0  RSION(s)        finde cALIBR0x, f_DXorY) 12f ((FilM_g SuE&&0inR,(f))(DP_OPTION_BYTE_ZERO(s) & 0x02) ? \
          finROts ascii */
  f][XorY])    ION(s)        X finde & , f0x0 0x040 0x02f ((finde cALIBR0x, f_D      (((a) >((sY finde & , f0x0 0x040 0x02f ((finde cALIBR0x, f_DL(a)    (((a) ((FilMr  Buc& 0x0 0x04)
#dee DP_OPTIaa pmN */
[(S_BYAUTO nd m640)]      ((s)->unBLd m MaWHITEFilM0x, f0 : 1f ((FilM_g SuE&&0inR,(f))(
#deeOUN_OPTION_BYTE_ZERO(s) & 0x02) ? \
          (inROt-aa pmNraC f][
	==A'B'      B04)
#deFilMWASDOWNL AD) >>, f0 : 1f ((FilM_g SuE&&0inR,(f))(
#deeOUN_OPTION_BYTE_ZERO(s) & 0x02) ? \
          (inROt-aa pmNraC f][1
	==A'D'      B04)
#deYAUTO nd mFilM0x, f0 : 1: 1f ((FilM_g SuE&&0inR,(f))(
#deeOUN_OPTION_BYTE_ZERO(s) & 0x02) ? \
          (inROt-aa pmNraC f][2
	==A'*'GAL(a) (((a) Reefinend   EOUTout limitL

/lTIM     s_TIM tOUTouts.   ((sassPr     ()((a) YNAs theseFULlu
s.  EOUTout limitLother than theseFULlu
s mne be YNA((a) afs
 a succ 0 full re <=n from  ((sassPr     ()3))
/*
 * ServoT Ma 409ShortTi */   4/
#de   ((s)->sTKversion_LE minurt, vary exp. r)->uiShortTid.
 */x2aa8
#de  )->uiShortTimeOu_LE0 minurtsAR    1 /* Linra mss     Warnings re <=n   from Toolkit 0) tD(s)3))
 TheseFULlu
s a= 1always currentACK >= 0) &&ea ms c */ons for Background               0ary exp. r)-GIa R         -1ary exp. rH 40W R         -2ary exp. rShortTid.
 */  -3    B04)
#IS_ONNde Eand.-4    B04)
UNKNOWN     */  -5   (((a) >= NFIG   
 */  -6    B04)
#dileIO.F   
 *-7* Backgroun)->ucis SET d */     (((a) >=uiExpACTIVE */2ary exp. rR & VEFilM
 */  3   1 /* LinCFG(     &ea ms n */

/*
 * DP_DATA CFG(NO_ENVIRONME&&d */     (((a)CFG(FileIOPde_        2ary exp. rCFG(FileIxpAD_        3    B04)
CFG(    _              ary exp. rCFG(FilMIOPde_        5   1 /* Lincas
#dileIO.F   ea ms n */

/*
 * DP_DATA TKBsdD(s)G(= Mg            (((a)TKBsdD(F  McMlTHOand.2   1 /* LinGesPr     OSURE_()ACKquiry flags toACKdicart,whichACKquries toAmake of the * Lin      Digi;
 Palette.  Eheyncan be OR'd together 

/multipleACKquirieons for BackgrouSta_              00     (((a)Sta_YAUTO         002ary exp. rSta_SCSIPOS       00 ary exp. rSta_sedLin        008ary exp. rSta_FilMS         010ary exp. rSta_finde         020ary exp. rSta_n)->uc        040ary exp. rfullStatIRIES     07f   1 /* LinI     compr 0  RSons for BackgroundN_RleIuE&D_fi_Rle       
#dackault; compr 0  be

e YNKdiIM for BackgroundN_Rle02) ? \
          1
#duncompr 0 ed, YNKdduncompr 0 ed for BackgrouRle02) ? \
          nd.2
#dcompr 0 ed, YNKddcompr 0 ed for Backgrou= Mg Su(s) _LEGAL(a0x0 0x0(i(a0x>=D  eOUNi(a0xE_L2GAL(a) (((a) I     TerminaBYTLns for BackgrouE&D_OFs)->uiExp       nd.
#dnorm
 e_EXPu
e terminaBYTL    nd.for BackgrouAB   _ ->uiExp       nd. 1
#dabTIM al
 e_EXPu
esACK queue   nd.for BackgrouSMA  _AB   
          nd.2
#dterminaBe laBestAC    , Yave queue 
 * DP_DATA T  McMlTHOa_LEGAL(a0x0 0x0(i(a0x>=D  eOUNi(a0xE_L2GAL(a)a) (         ID_o cNG  is u ed toAdetermint,whether we'r  connii ed toAtheOPTIONPalette via a CentronicLor           face.  EheUS_B     n */
 isOPTIONaNParal
e
 PTIM (CentronicL)ACf it is between           ID_o cNG  - 1.OPTIONIf the S_B     n */
 is YNA toAaFULlu
 from  ((    ID_o cNG  orOPTIONabov , thenAaFconnii YTL is made via the     

 * DP_DATA  ((    ID_o cNG    10ara) (((a) Toolkit func YTL prototypeons for Bifnd f
#diKUTIL_C
ex   n char   nd.f>((setConfigPath(voiE
;
ex   n voiE     nd. (( leep(   
;
ex   n          nd. ((F(s)Open(>= 0) &&*, char *
;
ex   n          nd. ((F(s)Read(>= 0) &&*, char *,    
;
ex   n          nd. ((F(s)Close(>= 0) &&*
;
ex   n unsign       >unoLEncode(char *, char *,    
;
ex   n voiE     nd. ((PathParse(char *, char *, char *, char *, char *
;
#NKdif   1Bifnd f
#d *IT_C
ex   n     >unsassPr     (>= 0) &&*,    ,    
;
ex   n     >= 0ownLoada pms(>= 0) &&*, char *,    
;
#NKdif   1Bifnd f
#dSNDP McC
ex   n     >unOSUrtE_EXPu
e(>= 0) &&*
;
ex   n     >unONKdPr     Params(>= 0) &&*
;
ex   n     >unTerminaBeE_EXPu
e(>= 0) &&*,    
;
#NKdif   1Bifnd f
#dStatIRcC
ex   n     >unGesPr     OSURE_(>= 0) &&*,    
;
#NKdif   1Bifnd f
#dSNDs)G(=
ex   n     >unONKdI    Data(>= 0) &&*,    , char *,   ,   
;
#NKdif   1Bifnd f
PATHPARS(=
ex   n voiE PathParse(char *, char *, char *, char *, char *
;
#NKdif   1Bifnd f
oLENClit(=
ex   n unsign       oLEncode(char *, char *,    
;
#NKdif   1 ( polaBOUT.asmns for ex   n voiE WaitF*/Ticks(unsign      
;
