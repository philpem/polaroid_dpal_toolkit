#
#	Makefile for SGI
#
#	MK/Carlsberg	930930
#
TKLIBFILE = ../sgiscsi/libtkscsi.a
TKLIB = -L../sgiscsi -ltkscsi
CFLAGS = -cckr -DSGI

toolkit:	CFGdump CFGmake

PrSend.obj: PrSend.asm moddef.inc
	masm -D$(M_model) -MX PrSend ;
	lib TKDP2000.LIB -+ $@ ;

DPinit.obj: DPinit.c dpalette.h dptkcent.h
	cl -c -A$(C_model) DPinit.c ;
	lib TKDP2000.LIB -+ $@ ;

DPinquir.obj: DPinquir.c dpalette.h dptkcent.h
	cl -c -A$(C_model) DPinquir.c ;
	lib TKDP2000.LIB -+ $@ ;

DPsndprm.obj: DPsndprm.c dpalette.h dptkcent.h
	cl -c -A$(C_model) DPsndprm.c ;
	lib TKDP2000.LIB -+ $@ ;

DPsndimg.obj: DPsndimg.c dpalette.h dptkcent.h
	cl -c -A$(C_model) DPsndimg.c ;
	lib TKDP2000.LIB -+ $@ ;

DPcomm.obj: DPcomm.c dpalette.h dptkcent.h
	cl -c -A$(C_model) DPcomm.c ;
	lib TKDP2000.LIB -+ $@ ;

DPtkUtil.obj: DPtkUtil.c dpalette.h dptkcent.h
	cl -c -A$(C_model) DPtkUtil.c ;
	lib TKDP2000.LIB -+ $@ ;

PathPars.obj: PathPars.c
	cl -c -A$(C_model) PathPars.c ;
	lib TKDP2000.LIB -+ $@ ;

RLEncode.obj: RLEncode.c tkhost.h
	cl -c -A$(C_model) RLEncode.c ;
	lib TKDP2000.LIB -+ $@ ;

CFGmake.o: CFGmake.c dpalette.h dptkcent.h tkhost.h
	cc $(CFLAGS) -c CFGmake.c

CFGmake: CFGmake.o $(TKLIBFILE)
	cc -o CFGmake CFGmake.o $(TKLIB)

CFGdump:	CFGdump.o $(TKLIBFILE)
	cc -o CFGdump CFGdump.o $(TKLIB)

CFGdump.o:	CFGdump.c dpalette.h dptkcent.h tkhost.h
	cc $(CFLAGS) -c CFGdump.c

