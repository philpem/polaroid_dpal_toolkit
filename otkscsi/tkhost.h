/*  DEC/CMS REPLACEMENT HISTORY, Element TKHOST.H */
/*  *2     3-MAY-1991 12:14:46 BROGNA "" */
/*  *1     5-NOV-1990 18:13:07 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element TKHOST.H */
/*  tkhost.h
 Copyright (C) 1989, 1989, 1990, 1991 Polaroid Corporation

    The host environment for the toolkit is declared here.  The toolkit
    is currently available for both the Centronics and SCSI interface to
    the Palette.  For these two flavors, the toolkit code itself is
    completely different.  The dpalette.h file, and the form of the interface
    functions to the toolkit, however, remain the same regardless of the
    interface to the Palette.  Within each of these two toolkit flavors,
    the toolkit code can be compiled for different host target systems.
    This is particularly true of the SCSI interface.  If you have any
    declarations which are host or compiler dependant, they should be placed
    in this file.

    We currently define two host environments; MSDOS or MACINTOSH_OS
        
    For MACINTOSH, there are additional environment declaraions for
    the LightSpeed C V3.01 compiler.  This is necessary because
    LSC, V3.01 is not quite ANSI..
        
/*  Environment under which the toolkit is compiled.  */
#ifndef MSDOS        
#define MSDOS         /*  include this if you are running on DOS     */
#endif

/* #define MACINTOSH_OS     /*  include this if you are running on the Mac */


#ifdef MACINTOSH_OS
#define LSCV3            /*  include this if using LightSpeed C V3.01   */
                         /*    on the Mac                               */


#ifdef  LSCV3
#define memmove( s, d, c)   movmem( d, s, c )   /* non-ANSI */
#endif
#endif
