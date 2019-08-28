/*  DEC/CMS REPLACEMENT HISTORY, Element TKINQUIR.H */
/*  *3     3-MAY-1991 12:14:52 BROGNA "" */
/*  *2    27-MAR-1991 15:24:51 BROGNA "Multiple Printer support" */
/*  *1     5-NOV-1990 18:13:18 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKINQUIR.H */
/*  tkinquir.h

    Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
		 
    declarations of the data included in the Palette SCSI inquire command

29-jun-90  vg       initial...
18-mar-91  jb		added IDENT_STRING "DP2SCSI"

*/

/* maximum size of an inquiry data block */
#define INQUIRY_BUFFER_SIZE  63

#define HEADER_SIZE         5

/* items in the inquiry data block */
#define INQ_DEVICE_TYPE             0
#define INQ_DEVICE_QUALIFIER        1
#define INQ_RESERVE_1               2
#define INQ_RESERVE_2               3
#define INQ_ADDITIONAL_LENGTH       4
#define INQ_RESERVE_3               5
#define INQ_RESERVE_4               6
#define INQ_RESERVE_5               7
#define INQ_IDENTIFICATION          8
#define INQ_PRODUCT_ID              16
#define INQ_REVISION_LEVEL          32
#define INQ_RESERVE_6               36
#define INQ_RESERVE_7               37
#define INQ_RESERVE_8               38
#define INQ_RESERVE_9               39
#define INQ_MAX_BUFFER_SIZE_MSB     40
#define INQ_MAX_BUFFER_SIZE_LSB     41
#define INQ_FILM_NO_MIN             42
#define INQ_FILM_NO_MAX             43
#define INQ_HRES_MIN_MSB            44
#define INQ_HRES_MIN_LSB            45
#define INQ_HRES_MAX_MSB            46
#define INQ_HRES_MAX_LSB            47
#define INQ_VRES_MIN_MSB            48
#define INQ_VRES_MIN_LSB            49
#define INQ_VRES_MAX_MSB            50
#define INQ_VRES_MAX_LSB            51
#define INQ_LUM_MIN                 52
#define INQ_LUM_MAX                 53
#define INQ_CBAL_MIN                54
#define INQ_CBAL_MAX                55
#define INQ_ETIME_MIN               56
#define INQ_ETIME_MAX               57
#define INQ_LDVAL_MIN               58
#define INQ_LDVAL_MAX               59
#define INQ_RESERVE_10              60
#define INQ_RESERVE_11              61
#define INQ_RESERVE_12              62


#define IDENT_STRING "DP2SCSI"
