/*  DEC/CMS REPLACEMENT HISTORY, Element CFGMAKE.C */
/*  *4     3-MAY-1991 12:04:32 BROGNA "" */
/*  *3    18-JAN-1991 22:47:42 GRABEL "changes to run under LSC on the MAC" */
/*  *2    18-JAN-1991 04:52:54 BROGNA "MSC 6.0x" */
/*  *1     7-NOV-1990 14:49:09 BROGNA "DEFAULT CONFIG FILE CREATOR... INITIAL." */
/*  DEC/CMS REPLACEMENT HISTORY, Element CFGMAKE.C */
/* CFGmake.c
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.

 18-jan-91  vg      added default film numbers
 */
#include <stdio.h>
#include "tkhost.h"
#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif


#include "dpalette.h" /* toolkit high level include file */

/* default film numbers for the various camera backs */
#define DEFAULT_PACK_FILM   11
#define DEFAULT_35MM_FILM    0
#define DEFAULT_AUTO_FILM    9
#define DEFAULT_4x5_FILM    14


main(void)
{
    DP_DATA     dp_data ;
    DP_DATA    *DPdata = &dp_data ;
    FILE *fh ;
    int  Size ;
    char *ptr ;
    char WedgeLength = 28 ;
    char *MetaFileExt = "CGM" ;
    
    printf("\nCFGmake.exe\n") ;
    printf("\n\tCreates a new configuration file called dpalette.cfg") ;
    printf("\n\tin the current directory.\n") ;

    /*
     * Clear the configuration data in the data structure, then populate with
     * configuration defaults.
     */
    ptr = (char *) DPdata ;
    for (Size = 0 ; Size < CONFIG_PARAM_BYTES ; Size++)
        *(ptr++) = 0 ;

    /*
     * Product ID.
    */
    strcpy(DP_DEVICE_NAME(DPdata), DIGITAL_PALETTE) ;
    
    /*
     * Default communications port.
    */
    DP_PORT(DPdata) = 1 ;

    /*
     * Default image size and placement.
    */
    DP_HOR_RES(DPdata) = 2048 ;
    DP_HOR_OFFSET(DPdata) = 0 ;
    DP_LINE_LENGTH(DPdata) = 2048 ;
    DP_VER_RES(DPdata) = 2048 ;
    DP_VER_OFFSET(DPdata) = 0 ;

    /*
     * Start Exposure parameters
    */
    DP_EXP_IMAGE_PROCESSING(DPdata) = SE_NO_PROCESSING ;
    DP_EXP_IMAGE_USAGE(DPdata) = SE_ALLOW_MULTIPLE_IMAGES ;
    DP_EXP_BUFFER_USAGE(DPdata) = SE_BUF_AS_RECVD ;
    DP_EXP_CALIBRATION(DPdata) = SE_NORMAL_CALIBRATION ;

    /*
     * Exposure settings.
    */
    DP_LIGHTEN_DARKEN(DPdata) = NORMAL ;
    DP_RED_EXP_TIME(DPdata) = 100 ;
    DP_GREEN_EXP_TIME(DPdata) = 100 ;
    DP_BLUE_EXP_TIME(DPdata) = 100 ;
    DP_RED_LUMINANT(DPdata) = 100 ;
    DP_GREEN_LUMINANT(DPdata) = 100 ;
    DP_BLUE_LUMINANT(DPdata) = 100 ;
    DP_RED_COLOR_BALANCE(DPdata) = NORMAL ;
    DP_GREEN_COLOR_BALANCE(DPdata) = NORMAL ;
    DP_BLUE_COLOR_BALANCE(DPdata) = NORMAL ;

    /*
     * default films are undefined initially.
     */
    DP_DEFAULT_FILM(DPdata, CAMERA_PACK) = DEFAULT_PACK_FILM ;
    DP_DEFAULT_FILM(DPdata, CAMERA_35MM) = DEFAULT_35MM_FILM ;
    DP_DEFAULT_FILM(DPdata, CAMERA_AUTO) = DEFAULT_AUTO_FILM ;
    DP_DEFAULT_FILM(DPdata, CAMERA_4X5) =  DEFAULT_4x5_FILM ;

    /*
     * Open the default configuration file.
     * Note that some systems do not distinguish between text and binary files.
     * In such cases change the mode argument to fopen() to "w".
    */
    if ((fh = fopen("dpalette.cfg", "wb")) == NULL)
    {
        printf("Config File open error.\n") ;
        return(CONFIGERR) ;
    }

    /*
     * Write out the configuration data in the data structure.
    */
    DP_SwapShorts(DPdata);

    ptr = (char *) DPdata ;
    for (Size = 0 ; Size < CONFIG_PARAM_BYTES ; ptr++, Size++)
        if (fputc((int) *ptr, fh) != (*ptr & 0x00ff))
            break ;
    if (Size < CONFIG_PARAM_BYTES)
    {
        fclose(fh) ;
        printf("Config File write error.\n") ;
        return(CONFIGERR) ;
    }

    /*
     * Default wedge lengths for camera adjustment user interface.
    */
    for (Size = 0 ; Size < (MAX_NUM_FILMS * 4) ; Size++)
        if (fputc((int) WedgeLength, fh) != (WedgeLength & 0x00ff))
            break ;
    if (Size < (MAX_NUM_FILMS * 4))
    {
        fclose(fh) ;
        printf("Config File write error.\n") ;
        return(CONFIGERR) ;
    }

    /*
     * Default Metafile extension for ImagePrint utility (and others).
    */
    for (Size = 0 ; Size < 4 ; Size++)
        if (fputc((int) MetaFileExt[Size], fh) !=
           (MetaFileExt[Size] & 0x00ff))
            break ;
    fclose(fh) ;
    if (Size < 4)
    {
        printf("Config File write error.\n") ;
        return(CONFIGERR) ;
    }
    
    return(0) ;
}
