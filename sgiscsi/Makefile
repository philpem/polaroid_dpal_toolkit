#
#	Makefile for SGI version.
#
#	MK/Carlsberg	930930
#
CFLAGS = -cckr -DSGI -O2

libtkscsi.a:	dpalette.h tkhost.h scdpinit.o scdpinqu.o scdpsprm.o \
		scdpsimg.o scdpscmd.o scdprerr.o PathPars.o \
		RLEncode.o dptkutil.o scdpsgi.o

dpalette.h: ../toolkit/dpalette.h
	cp  ../toolkit/dpalette.h .

tkhost.h: ../toolkit/tkhost.h
	cp  ../toolkit/tkhost.h .

scdpinit.o: scdpinit.c dpalette.h tkhost.h
	cc $(CFLAGS) -c scdpinit.c
	ar -r libtkscsi.a $@

scdpinqu.o: scdpinqu.c dpalette.h tkhost.h
	cc $(CFLAGS) -c  scdpinqu.c
	ar -r libtkscsi.a $@

scdpsprm.o: scdpsprm.c dpalette.h tkhost.h
	cc $(CFLAGS) -c  scdpsprm.c
	ar -r libtkscsi.a $@

scdpsimg.o: scdpsimg.c dpalette.h tkhost.h
	cc $(CFLAGS) -c  scdpsimg.c
	ar -r libtkscsi.a $@

scdpscmd.o: scdpscmd.c dpalette.h tkhost.h
	cc $(CFLAGS) -c  scdpscmd.c
	ar -r libtkscsi.a $@

scdprerr.o: scdprerr.c dpalette.h tkhost.h
	cc $(CFLAGS) -c  scdprerr.c
	ar -r libtkscsi.a $@

### for Macintosh, this file is not used, no need to include it ####
PathPars.o: PathPars.c
	cc $(CFLAGS) -c  PathPars.c
	ar -r libtkscsi.a $@

PathPars.c: ../toolkit/PathPars.c
	cp ../toolkit/PathPars.c .


RLEncode.c: ../toolkit/RLEncode.c
	cp ../toolkit/RLEncode.c .

RLEncode.o: RLEncode.c tkhost.h
	cc $(CFLAGS) -c  RLEncode.c
	ar -r libtkscsi.a $@

dptkutil.c: ../toolkit/dptkutil.c
	cp ../toolkit/dptkutil.c .

dptkutil.o: dptkutil.c dpalette.h
	cc $(CFLAGS) -c  dptkutil.c
	ar -r libtkscsi.a $@

moddef.inc: ../toolkit/moddef.inc
	cp ../toolkit/moddef.inc .

#PolaTime.asm: ../toolkit/PolaTime.asm
#	cp ../toolkit/PolaTime.asm .

#PolaTime.o: PolaTime.asm moddef.inc
#	masm -D$(M_model) -MX PolaTime
#	ar -r libtkscsi.a $@

####
#### host and SCSI adaptor specific files:  Choose ONLY one of the following:
####

#### For Macintosh users:
# scdpmac.o: dpalette.h scdpmac.c sclowlvl.h
#	cc $(CFLAGS) -c  scdpmac.c
#	ar -r libtkscsi.a $@

#### For DOS systems using the NCR ADP/S adaptor
#scdpncr.o: scdpncr.c dpalette.h sclowlvl.h
#	cc $(CFLAGS) -c  scdpncr.c
#	ar -r libtkscsi.a $@

#### For DOS systems using the IBM/PS2 SCSI adaptor
#scdpps2.o: scdpps2.c dpalette.h sclowlvl.h tkinquir.h
#	cc $(CFLAGS) -c  scdpps2.c
#	ar -r libtkscsi.a $@

#### For SGI systems
scdpsgi.o: scdpsgi.c dpalette.h sclowlvl.h tkinquir.h
	cc $(CFLAGS) -c  scdpsgi.c
	ar -r libtkscsi.a $@

#### For DOS systems using the Future Domain SCSI adaptor
#### Note: the oemscsi?.o files are object files which are supplied in
####       the Future Domain developers toolkit.  The following object
####       modules are supplied with the Digital Palette Toolkit:
####            futurdom/oemscsil.obj  Large Memory Model
####            futurdom/oemscsim.obj  Medium Memeory Model
####            futurdom/oemscsi.obj   Small Memory Model
####       The complete Future Domain toolkit must be acquired from Future
####       Domain.

FD_OBJ = futurdom/oemscsil.o

scdpfdom.o: scdpfdom.c sclowlvl.h
	cc $(CFLAGS) -c  scdpfdom.c
	ar -r libtkscsi.a $@
	ar -r libtkscsi.a $(FD_OBJ)
