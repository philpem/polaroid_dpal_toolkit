#  DEC/CMS REPLACEMENT HISTORY, Element SAMPLES.MS
#  *4     3-MAY-1991 12:11:56 BROGNA ""
#  *3    18-JAN-1991 17:20:41 CAMPBELL_T "Added dpabort.c"
#  *2    29-NOV-1990 16:59:12 BROGNA "Version 2.21"
#  *1    15-OCT-1990 10:17:23 BROGNA "SAMPLES MAKE FILE"
#  DEC/CMS REPLACEMENT HISTORY, Element SAMPLES.MS
# Toolkit -- Make file
#
#    Microsoft make utility make file for building the toolkit sources.
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

TKLIB = ..\toolkit\tkdp2000.lib
OBJS = demosup.obj sleep.obj

demosup.obj  : demosup.c demosup.h ..\toolkit\dpalette.h
   cl -c -A$(C_model) -I..\toolkit demosup.c ;

sleep.obj    : sleep.c
   cl -c -A$(C_model) sleep.c ;

apic.obj     : apic.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit apic.c ;

apic.exe     : apic.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 apic.obj $(OBJS), apic, /map, $(TKLIB) ;

bdparm.obj   : bdparm.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit bdparm.c ;

bdparm.exe   : bdparm.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 bdparm.obj $(OBJS), bdparm, /map, $(TKLIB) ;

timeit.obj   : timeit.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit timeit.c ;

timeit.exe   : timeit.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 timeit.obj $(OBJS), timeit, /map, $(TKLIB) ;

tkstat.obj   : tkstat.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit tkstat.c ;

tkstat.exe   : tkstat.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 tkstat.obj $(OBJS), tkstat, /map, $(TKLIB) ;

tkfilm.obj   : tkfilm.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit tkfilm.c ;

tkfilm.exe   : tkfilm.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 tkfilm.obj $(OBJS), tkfilm, /map, $(TKLIB) ;

npics.obj    : npics.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit npics.c ;

npics.exe    : npics.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 npics.obj $(OBJS), npics, /map, $(TKLIB) ;

aspect.obj   : aspect.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit aspect.c ;

aspect.exe   : aspect.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 aspect.obj $(OBJS), aspect, /map, $(TKLIB) ;

mkerror.obj  : mkerror.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit mkerror.c ;

mkerror.exe  : mkerror.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 mkerror.obj $(OBJS), mkerror, /map, $(TKLIB) ;

mosaic.obj   : mosaic.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit mosaic.c ;

mosaic.exe   : mosaic.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 mosaic.obj $(OBJS), mosaic, /map, $(TKLIB) ;

backg.obj    : backg.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit backg.c ;

backg.exe    : backg.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 backg.obj $(OBJS), backg, /map, $(TKLIB) ;

dpabort.obj  : dpabort.c demosup.obj sleep.obj ..\toolkit\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\toolkit dpabort.c ;

dpabort.exe    : dpabort.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 dpabort.obj $(OBJS), dpabort, /map, $(TKLIB) ;
