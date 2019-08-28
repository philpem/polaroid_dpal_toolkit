/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPERRS.H */
/*  *2     3-MAY-1991 12:12:03 BROGNA "" */
/*  *1     5-NOV-1990 17:48:08 GRABEL "INITIAL" */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPERRS.H */
/*      scdperrs.h

 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation

    Error declarations for the Digital Palette SCSI
        
29-jun-90  vg       initial...
 3-aug-90  vg       moved the NOERR declaration here


*/

/* NOERR is returned by the low level scsi functions if all went well */
#define NOERR       0


/* error conditions which are placed in sense key during a
   request sense bytes command
*/
#define SCS_NO_SENSE            0x00
#define SCS_NOT_READY           0x02
#define SCS_HARDWARE_ERROR      0x04
#define SCS_ILLEGAL_REQUEST     0x05
#define SCS_UNIT_ATTENTION      0x06
#define SCS_ABORTED_COMMAND     0x0B

/* additional error sense codes, are placed in bytes 8 and 9 of the
   request sense bytes
*/

/* for sense key = NO SENSE */
#define SCS_NO_ADD_SENSE                0x2000

/* for sense key = HARDWARE ERROR */
#define SCS_DIAG_FAILURE                0x2400
#define SCS_MEMORY_FAILURE              0x2401
#define SCS_VIDEO_BUFFER_FAILURE        0x2402
#define SCS_VIDEO_DATA_FAILURE          0x2403
#define SCS_GENERAL_IO_FAILURE          0x2404
#define SCS_CHECKSUM_ERROR              0x2405
#define SCS_POWER_ON_FAILURE            0x2406
#define SCS_FILTER_WHEEL_JAM            0x2407    
#define SCS_BAD_FILTER_POSITION         0x2408

#define SCS_FILM_DIRTY                  0x240B

#define SCS_CAL_ERR0                    0x2420
#define SCS_CAL_ERR1                    0x2421
#define SCS_CAL_ERR2                    0x2422
#define SCS_CAL_ERR3                    0x2423
#define SCS_CAL_ERR4                    0x2424
#define SCS_CAL_ERR5                    0x2425
#define SCS_CAL_ERR6                    0x2426
#define SCS_CAL_ERR7                    0x2427
#define SCS_CAL_ERR8                    0x2428



#define SCS_UNKNOWN_HWERR               0x24FF

/* for sense key = ILLEGAL REQUEST */
#define SCS_INVALID_CDB                 0x2500
#define SCS_INVALID_CMD_OPCODE          0x2501
#define SCS_RESERVED_BIT_SET            0x2502
#define SCS_UNSUPPORTED_FUNCTION        0x2503
#define SCS_REQ_LENGTH_INVALID          0x2504
#define SCS_RGB_COLOR_INVALID           0x2505
#define SCS_FLAG_LINK_ERROR             0x2506
#define SCS_PRINT_NO_START              0x2507
#define SCS_PRINT_BAD_LENGTH            0x2508
#define SCS_TERMINATE_NO_START          0x2509
#define SCS_COLOR_TAB_INVALID           0x250A
#define SCS_INVALID_LUN                 0x250B
#define SCS_START_NO_TERMINATE          0x250C

#define SCS_INVALID_FIELD               0x2540
#define SCS_INVALID_FUNCTION            0x2541
#define SCS_LENGTH_OUT_OF_RANGE         0x2542
#define SCS_FILMNO_NOT_AVAILABLE        0x2543
#define SCS_FILMNO_OUT_OF_RANGE         0x2544
#define SCS_HRES_OUT_OF_RANGE           0x2545
#define SCS_HOFFS_OUT_OF_RANGE          0x2546
#define SCS_LINE_LENGTH_OUT_OF_RANGE    0x2547
#define SCS_VRES_OUT_OF_RANGE           0x2548
#define SCS_VOFFS_OUT_OF_RANGE          0x2549
#define SCS_LUMRED_OUT_OF_RANGE         0x254A
#define SCS_LUMGRN_OUT_OF_RANGE         0x254B
#define SCS_LUMBLU_OUT_OF_RANGE         0x254C
#define SCS_CBALRED_OUT_OF_RANGE        0x254D
#define SCS_CBALGRN_OUT_OF_RANGE        0x254E
#define SCS_CBALBLU_OUT_OF_RANGE        0x254F
#define SCS_EXPTIMRED_OUT_OF_RANGE      0x2550
#define SCS_EXPTIMGRN_OUT_OF_RANGE      0x2551
#define SCS_EXPTIMBLU_OUT_OF_RANGE      0x2552
#define SCS_LITDRK_OUT_OF_RANGE         0x2553
#define SCS_DCONV_OUT_OF_RANGE          0x2554
#define SCS_EXEMODE_OUT_OF_RANGE        0x2555
#define SCS_IMGENH_OUT_OF_RANGE         0x2556
#define SCS_CAMADJ_INVALID_PARAMETER    0x2557
#define SCS_PRINT_BADLINENO             0x2558
#define SCS_CALCTRL_OUT_OF_RANGE        0x2559
#define SCS_IMGCTRL_OUT_OF_RANGE        0x255A

#define SCS_FILMTAB_BADDATA             0x255B
#define SCS_FILMTAB_BADSIZE             0x255C
#define SCS_SWEEPMODE_BAD               0x255D

#define SCS_IMGHEIGHT_OUT_OF_RANGE      0x255E
#define SCS_SERVO_OUT_OF_RANGE          0x255F

/* sense key = UNIT ATTENTION */
#define SCS_POWER_ON                    0x2600

/* sense key = ABORTED COMMAND */
#define SCS_BAD_MESSAGE_RETRY           0x2B00
#define SCS_MESSAGE_PARITY_ERROR        0x2B01
#define SCS_INT_PARITY_ERROR            0x2B02


/*  The 'out of film' condition is one of the few errors from the Palette
    which is not reported with the 'additional sense bytes' of the extended
    sense.  Instead, End of Media (EOM) bit of byte 2 in the Request
    Sense bytes is set if out of film
*/
#define EOM_BIT 0x40

#define EOM_ERRCLASS    HARDWERR
#define EOM_ERRNUMBER   0x2701          /* a rather arbitrary number. Picked
                                           only to avoid any conflict
                                        */
#define EOM_MESSAGE     "** Out of Film **"
#define EOM_TEST(s)     (s[2] & EOM_BIT)

#define SC_UNKNOWNPORT  0x2702          /* again, a rather arbitrary number
                                           indicates that the supplied DP_PORT
                                           is invalid
                                        */

/* Error messages which can be returned by the toolkit but are not
   associated with Extended Sense Errors
*/
#define NoErrorsReported  "Digital Palette reports no errors"
#define Incommunicado     "Digital Palette is Busy, Disconnected, or Off"
#define NoBufferSpace     "Digital Palette Buffer is Full" 
#define ErrorNotCleared   "Digital Palette Error.  Error Not Cleared"
#define ErrorNotFound     "Unknown Digital Palette Error"
#define BadPortNumber     "Bad Port Number"

/* warning messages generated by this code */
#define ExposureInBox    "Digital Palette Exposure in progress"
#define ExposureComplete "Digital Palette Exposure Complete. REMOVE FILM!"
#define UnknownError     "Unknown Error" 
