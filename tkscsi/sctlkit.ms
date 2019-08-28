#  DEC/CMS REPLACEMENT HISTORY, Element SCTLKIT.MS
#  *6     6-MAY-1991 16:52:47 BROGNA "Future Domain inclusion in Toolkit"
#  *5     3-MAY-1991 12:14:15 BROGNA ""
#  *4    18-JAN-1991 17:21:27 CAMPBELL_T "Added dpabort.c"
#  *3    29-NOV-1990 16:59:21 BROGNA "Version 2.21"
#  *2     7-NOV-1990 14:47:29 BROGNA "Added ISV build of CFGdump.exe and CFGmake.exe"
#  *1     5-NOV-1990 18:12:44 GRABEL "initial..."
#  DEC/CMS REPLACEMENT HISTORY, Element SCTLKIT.MS
# SCSI Toolkit -- Prototype Make file
#
#    Microsoft make utility make file for building the toolkit sources.
#    Use this as a starting point to build your own make file.  
# Memory model support
#
# Large
C_model=L
M_model=LARGE
#
# Medium
#C_model=M
#M_model=MEDIUM
#
# Compact
#C_model=C
#M_model=COMPACT
#
# Small
#C_model=S
#M_model=SMALL
#

dpalette.h     : ..\toolkit\dpalette.h
     copy  ..\toolkit\dpalette.h

tkhost.h       : ..\toolkit\tkhost.h
     copy  ..\toolkit\tkhost.h

scdpinit.obj   : scdpinit.c dpalette.h tkhost.h
     cl -c -A$(C_model) scdpinit.c ;
     lib tkscsi.lib -+ $@ ;

scdpinqu.obj : scdpinqu.c dpalette.h tkhost.h
     cl -c -A$(C_model) scdpinqu.c ;
     lib tkscsi.lib -+ $@ ;

scdpsprm.obj : scdpsprm.c dpalette.h tkhost.h
     cl -c -A$(C_model) scdpsprm.c ;
     lib tkscsi.lib -+ $@ ;

scdpsimg.obj : scdpsimg.c dpalette.h tkhost.h
     cl -c -A$(C_model) scdpsimg.c ;
     lib tkscsi.lib -+ $@ ;

scdpscmd.obj : scdpscmd.c dpalette.h tkhost.h
     cl -c -A$(C_model) scdpscmd.c ;
     lib tkscsi.lib -+ $@ ;

scdprerr.obj : scdprerr.c dpalette.h tkhost.h
     cl -c -A$(C_model) scdprerr.c ;
     lib tkscsi.lib -+ $@ ;

### for Macintosh, this file is not used, no need to include it ####
pathpars.c   : ..\toolkit\pathpars.c
     copy ..\toolkit\pathpars.c

pathpars.obj : pathpars.c
     cl -c -A$(C_model) PathPars.c ;
     lib tkscsi.lib -+ $@ ;

RLEncode.c   : ..\toolkit\RLEncode.c
     copy ..\toolkit\RLEncode.c

RLEncode.obj : RLEncode.c tkhost.h
     cl -c -A$(C_model) RLEncode.c ;
     lib tkscsi.lib -+ $@ ;

dptkutil.c   : ..\toolkit\dptkutil.c
     copy ..\toolkit\dptkutil.c

dptkutil.obj : dptkutil.c dpalette.h
     cl -c -A$(C_model) dptkutil.c ;
     lib tkscsi.lib -+ $@ ;

moddef.inc   : ..\toolkit\moddef.inc
     copy ..\toolkit\moddef.inc

PolaTime.asm : ..\toolkit\PolaTime.asm
     copy ..\toolkit\PolaTime.asm

PolaTime.obj : PolaTime.asm moddef.inc
     masm -D$(M_model) -MX PolaTime ;
     lib tkscsi.lib -+ $@ ;

####
#### host and SCSI adaptor specific files:  Choose ONLY one of the following:
####

#### For Macintosh users:
# scdpmac.obj : dpalette.h scdpmac.c sclowlvl.h
#     cl -c -A$(C_model) scdpmac.c ;
#     lib tkscsi.lib -+ $@ ;

#### For DOS systems using the NCR ADP/S adaptor
#scdpncr.obj : scdpncr.c dpalette.h sclowlvl.h
#     cl -c -A$(C_model) scdpncr.c ;
#     lib tkscsi.lib -+ $@ ;

#### For DOS systems using the IBM/PS2 SCSI adaptor
#scdpps2.obj : scdpps2.c dpalette.h sclowlvl.h tkinquir.h
#     cl -c -A$(C_model) scdpps2.c ;
#     lib tkscsi.lib -+ $@ ;

#### For DOS systems using the Future Domain SCSI adaptor
#### Note: the oemscsi?.obj files are object files which are supplied in
####       the Future Domain developers toolkit.  The following object
####       modules are supplied with the Digital Palette Toolkit:
####            futurdom\oemscsil.obj  Large Memory Model
####            futurdom\oemscsim.obj  Medium Memeory Model
####            futurdom\oemscsi.obj   Small Memory Model
####       The complete Future Domain toolkit must be acquired from Future
####       Domain.

FD_OBJ = futurdom\oemscsil.obj

scdpfdom.obj : scdpfdom.c sclowlvl.h
     cl -c -A$(C_model) scdpfdom.c ;
     lib tkscsi.lib -+ $@ ;
     lib tkscsi.lib -+ $(FD_OBJ) ;
