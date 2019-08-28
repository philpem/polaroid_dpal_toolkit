all:	apic bdparm timeit tkstat tkfilm npics aspect mosaic backg dpabort

TKLIBFILE = ../sgiscsi/libtkscsi.a
TKLIB = -L../sgiscsi -ltkscsi -lds -lm -lmalloc
CFLAGS = -cckr -DSGI
LFLAGS = -g
OBJS = demosup.o sleep.o

demosup.o: demosup.c demosup.h ../sgiscsi/dpalette.h
	cc -c $(CFLAGS) -I../sgiscsi demosup.c

sleep.o: sleep.c
	cc -c $(CFLAGS) sleep.c

apic.o: apic.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi apic.c

apic: apic.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o apic apic.o $(OBJS) $(TKLIB)

bdparm.o: bdparm.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi bdparm.c

bdparm: bdparm.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o bdparm bdparm.o $(OBJS) $(TKLIB)

timeit.o: timeit.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi timeit.c

timeit: timeit.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o timeit timeit.o $(OBJS) $(TKLIB)

tkstat.o: tkstat.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi tkstat.c

tkstat: tkstat.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o tkstat tkstat.o $(OBJS) $(TKLIB)

tkfilm.o: tkfilm.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi tkfilm.c

tkfilm: tkfilm.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o tkfilm tkfilm.o $(OBJS) $(TKLIB)

npics.o: npics.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi npics.c

npics: npics.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o npics npics.o $(OBJS) $(TKLIB)

aspect.o: aspect.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi aspect.c

aspect: aspect.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o aspect aspect.o $(OBJS) $(TKLIB)

mosaic.o: mosaic.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi mosaic.c

mosaic: mosaic.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o mosaic mosaic.o $(OBJS) $(TKLIB)

backg.o: backg.c demosup.o sleep.o ../sgiscsi/dpalette.h demosup.h
	cc -c $(CFLAGS) -I../sgiscsi backg.c

backg: backg.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o backg backg.o $(OBJS) $(TKLIB)

dpabort.o: dpabort.c ../sgiscsi/dpalette.h
	cc -c $(CFLAGS) -I../sgiscsi dpabort.c

dpabort: dpabort.o $(OBJS) $(TKLIBFILE)
	cc $(LFLAGS) -o dpabort dpabort.o $(OBJS) $(TKLIB)

