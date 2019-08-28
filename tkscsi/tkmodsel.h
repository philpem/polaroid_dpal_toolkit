/*  DEC/CMS REPLACEMENT HISTORY, Element TKMODSEL.H */
/*  *2     3-MAY-1991 12:14:58 BROGNA "" */
/*  *1     5-NOV-1990 18:13:29 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKMODSEL.H */
/*      tkmodsel.h

 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation

    Data contained in the Palette SCSI mode select command

29-jun-90  vg       initial....
*/


#define PARAMETER_LENGTH             4
#define SELECT_DESCRIPTOR_SIZE      39
#define SELECT_BUFFER_SIZE          ( 4 + 39 )  /* combo of the above sizes */

#define PARAMETER_BYTE0     0
#define PARAMETER_BYTE1     1
#define PARAMETER_BYTE2     2
#define DESCRIPTOR_LENGTH   3

#define FILM_NUMBER         0 + PARAMETER_LENGTH
#define RESERVED_1          1 + PARAMETER_LENGTH
#define HRES_MSB            2 + PARAMETER_LENGTH
#define HRES_LSB            3 + PARAMETER_LENGTH
#define HOFFSET_MSB         4 + PARAMETER_LENGTH
#define HOFFSET_LSB         5 + PARAMETER_LENGTH
#define LINE_LENGTH_MSB     6 + PARAMETER_LENGTH
#define LINE_LENGTH_LSB     7 + PARAMETER_LENGTH
#define RESERVED_2          8 + PARAMETER_LENGTH
#define VRES_MSB            9 + PARAMETER_LENGTH
#define VRES_LSB            10 + PARAMETER_LENGTH
#define VOFFSET_MSB         11 + PARAMETER_LENGTH
#define VOFFSET_LSB         12 + PARAMETER_LENGTH
#define RESERVED_3          13 + PARAMETER_LENGTH
#define LUM_RED             14 + PARAMETER_LENGTH
#define LUM_GREEN           15 + PARAMETER_LENGTH
#define LUM_BLUE            16 + PARAMETER_LENGTH
#define RESERVED_4          17 + PARAMETER_LENGTH
#define CBAL_RED            18 + PARAMETER_LENGTH
#define CBAL_GREEN          19 + PARAMETER_LENGTH
#define CBAL_BLUE           20 + PARAMETER_LENGTH
#define RESERVED_5          21 + PARAMETER_LENGTH
#define ETIME_RED           22 + PARAMETER_LENGTH
#define ETIME_GREEN         23 + PARAMETER_LENGTH
#define ETIME_BLUE          24 + PARAMETER_LENGTH
#define RESERVED_6          25 + PARAMETER_LENGTH
#define LTDRK_VALUE         26 + PARAMETER_LENGTH

#define IMAGE_HEIGHT_MSB            27 + PARAMETER_LENGTH
#define IMAGE_HEIGHT_LSB            28 + PARAMETER_LENGTH

#define SERVO_MODE                  29 + PARAMETER_LENGTH

#define DATA_CONVERSION_MODE        30 + PARAMETER_LENGTH
#define COMMAND_BUFFER_USAGE        31 + PARAMETER_LENGTH
#define CALIBRATION_CONTROL         32 + PARAMETER_LENGTH
#define IMAGE_ENHANCEMENT_MODE      33 + PARAMETER_LENGTH
#define IMAGE_ENHANCEMENT_MODE_2    34 + PARAMETER_LENGTH
#define IMAGE_CONTROL               35 + PARAMETER_LENGTH
#define RESERVED_11                 36 + PARAMETER_LENGTH
#define RESERVED_12                 37 + PARAMETER_LENGTH
#define RESERVED_13                 38 + PARAMETER_LENGTH
