/*  DEC/CMS REPLACEMENT HISTORY, Element TKMODSEN.H */
/*  *2     3-MAY-1991 12:15:04 BROGNA "" */
/*  *1     5-NOV-1990 18:13:41 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKMODSEN.H */
/*      tkmodsen.h

 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation

    Data contained in the Palette SCSI mode sense command

29-jun-90  vg   initial...
 8-aug-90  vg   added power on default declarations
*/

/*  data contained in the mode sense data buffer */
#define SENSE_BUFFER_SIZE           61

#define MS_SENSE_DATA_LENGTH           0
#define MS_RESERVED_0                  1
#define MS_RESERVED_1                  2
#define MS_RESERVED_2                  3
#define MS_CURRENT_BUFFER_SIZE_MSB     4
#define MS_CURRENT_BUFFER_SIZE_LSB     5
#define MS_FILM_NUMBER                 6
#define MS_FILM_ASPECT_RATIO_HORZ      7
#define MS_FILM_ASPECT_RATIO_VERT      8
#define MS_RESERVED_3                  9
#define MS_HRES_MSB                    10
#define MS_HRES_LSB                    11
#define MS_HOFFSET_MSB                 12
#define MS_HOFFSET_LSB                 13
#define MS_LINE_LENGTH_MSB             14
#define MS_LINE_LENGTH_LSB             15
#define MS_RESERVED_4                  16
#define MS_VRES_MSB                    17
#define MS_VRES_LSB                    18
#define MS_VOFFSET_MSB                 19
#define MS_VOFFSET_LSB                 20
#define MS_RESERVED_5                  21
#define MS_LUM_RED                     22
#define MS_LUM_GREEN                   23
#define MS_LUM_BLUE                    24
#define MS_RESERVED_6                  25
#define MS_CBAL_RED                    26
#define MS_CBAL_GREEN                  27
#define MS_CBAL_BLUE                   28
#define MS_RESERVED_7                  29
#define MS_ETIME_RED                   30
#define MS_ETIME_GREEN                 31
#define MS_ETIME_BLUE                  32
#define MS_RESERVED_8                  33
#define MS_LTDRK_VALUE                 34

#define MS_IMAGE_HEIGHT_MSB            35
#define MS_IMAGE_HEIGHT_LSB            36

#define MS_SERVO_MODE                  37

#define MS_DATA_CONVERSION_MODE        38
#define MS_CMDBUF_USAGE                39
#define MS_CALIBRATION_CONTROL         40
#define MS_IMAGE_ENHANCEMENT           41
#define MS_RESERVED_14                 42
#define MS_IMAGE_CONTROL               43
#define MS_RESERVED_16                 44
#define MS_CURRENT_CAMERA_BACK         45
#define MS_RESERVED_17                 58
#define MS_RESERVED_18                 59
#define MS_RESERVED_19                 60

#define CBNAMESIZE  ( MS_RESERVED_17 - MS_CURRENT_CAMERA_BACK )


