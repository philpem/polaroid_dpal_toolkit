#  DEC/CMS REPLACEMENT HISTORY, Element TOOLKIT.MS
#  *8     8-AUG-1991 15:01:28 BIELEFELD "updating for aspi"
#  *7     3-MAY-1991 12:16:08 BROGNA ""
#  *6     1-APR-1991 18:11:02 BROGNA "Toolkit enhancments"
#  *5    29-NOV-1990 16:59:43 BROGNA "Version 2.21"
#  *4     7-NOV-1990 14:47:46 BROGNA "Added ISV build of CFGdump.exe and CFGmake.exe"
#  *3     5-NOV-1990 17:38:48 GRABEL "added tkhost.h dependancies"
#  *2    15-OCT-1990 10:16:38 BROGNA ""
#  *1    10-OCT-1990 16:58:50 BROGNA "Version 2.1"
#  DEC/CMS REPLACEMENT HISTORY, Element TOOLKIT.MS
# Toolkit -- Make file
#
#    Microsoft make utility make file for building the toolkit sources.
#

#
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

PrSend.obj   : PrSend.asm moddef.inc
     masm -D$(M_model) -MX PrSend ;
     lib TKDP2000.LIB -+ $@ ;

PolaTime.obj : PolaTime.asm moddef.inc
     masm -D$(M_model) -MX PolaTime ;
     lib TKDP2000.LIB -+ $@ ;

DPinit.obj   : DPinit.c DPalette.h dptkcent.h
     cl -c -A$(C_model) DPinit.c ;
     lib TKDP2000.LIB -+ $@ ;

DPinquir.obj : DPinquir.c DPalette.h dptkcent.h
     cl -c -A$(C_model) DPinquir.c ;
     lib TKDP2000.LIB -+ $@ ;

DPsndprm.obj : DPsndprm.c DPalette.h dptkcent.h
     cl -c -A$(C_model) DPsndprm.c ;
     lib TKDP2000.LIB -+ $@ ;

DPsndimg.obj : DPsndimg.c DPalette.h dptkcent.h
     cl -c -A$(C_model) DPsndimg.c ;
     lib TKDP2000.LIB -+ $@ ;

DPcomm.obj   : DPcomm.c DPalette.h dptkcent.h
     cl -c -A$(C_model) DPcomm.c ;
     lib TKDP2000.LIB -+ $@ ;

DPtkUtil.obj : DPtkUtil.c DPalette.h dptkcent.h
     cl -c -A$(C_model) DPtkUtil.c ;
     lib TKDP2000.LIB -+ $@ ;

PathPars.obj : PathPars.c
     cl -c -A$(C_model) PathPars.c ;
     lib TKDP2000.LIB -+ $@ ;

RLEncode.obj : RLEncode.c tkhost.h
     cl -c -A$(C_model) RLEncode.c ;
     lib TKDP2000.LIB -+ $@ ;

CFGmake.obj : CFGmake.c DPalette.h dptkcent.h tkhost.h
     cl -c -A$(C_model) CFGmake.c ;

CFGmake.exe : CFGmake.obj TKDP2000.LIB
     link /STACK:8096 /EXEpack /NOE CFGmake.obj, $@, /map, TKDP2000.LIB ;

CFGdump.obj : CFGdump.c DPalette.h dptkcent.h tkhost.h
     cl -c -A$(C_model) CFGdump.c ;

CFGdump.exe : CFGdump.obj TKDP2000.LIB
     link /STACK:8096 /EXEpack /NOE CFGdump.obj, $@, /map, TKDP2000.LIB ;
