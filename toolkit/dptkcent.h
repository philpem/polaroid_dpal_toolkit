/*  DEC/CMS REPLACEMENT HISTORY, Element DPTKCENT.H */
/*  *8     3-MAY-1991 12:06:45 BROGNA "" */
/*  *7    18-JAN-1991 04:54:11 BROGNA "MSC 6.0x" */
/*  *6    29-NOV-1990 16:58:38 BROGNA "Version 2.21" */
/*  *5    28-NOV-1990 11:51:57 BROGNA "Dual TimeOut levels" */
/*  *4    16-NOV-1990 12:10:32 BROGNA "Multi-Printer support" */
/*  *3     7-NOV-1990 14:42:58 BROGNA "Removed CONFIG_PARAM_BYTES" */
/*  *2    15-OCT-1990 10:15:12 BROGNA "" */
/*  *1    10-OCT-1990 16:58:18 BROGNA "Version 2.1" */
/*  DEC/CMS REPLACEMENT HISTORY, Element DPTKCENT.H */
/* TKCENT.H
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
 *
 * Include file for Polaroid Digital Palette Centronics Toolkit sources.
 *
 */

#define TKCENT_H

/*
 * Sizes and Limits.
 */
#define PARAM_BYTES          2000   /* byte size of parameters to send */
#define DP_CMD_PACKET_SIZE      5

/*
 * Internal toolkit waitforbuffer flag.  This is NOT the application setting.
 */
#define TKWAIT                  1   /* wait for buffer to become available */
#define TKNOWAIT                0   /* don't wait, return BUFFERWARNING */

/*
 * Value for Time out Limit on Initialization.
 */
#define SHORT_TIMEOUT      0x0037 /* 3 seconds */

/*
 * DP_DATA structure Macros
 */
#define DP_TIMEOUT(s)                 ((s)->uiTimeOut)
#define DP_COMMAND(s)                 ((s)->sDPcommand)
#define DP_DATA_POINTER(s)            ((s)->cpDataPtr)
#define DP_MESSAGE_POINTER(s)         ((s)->cpMsgPtr)
#define DP_DATA_BYTES(s)              ((s)->iNumDataBytes)

#ifndef DPINQUIR_C
extern int DP_ConfirmBuffer(DP_DATA *, int, int) ;
extern int DP_ExposureWarning(DP_DATA *) ;
#endif

#ifndef DPCOMM_C
extern int DP_ClearError(void) ;
extern int DP_Reset(void) ;
extern int DP_Send(DP_DATA *, unsigned int) ;
extern int DP_Get(DP_DATA *, unsigned int) ;
#endif

/* PrSend.asm
 */
extern int PrInit(int) ;
extern int PrSend(char *, int, int, int, unsigned int) ;
extern int PrGet(unsigned int) ;
extern int PrErrors(void) ;
