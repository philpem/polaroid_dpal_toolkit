#  DEC/CMS REPLACEMENT HISTORY, Element SCSAMPLE.MS
#  *4     3-MAY-1991 12:13:53 BROGNA ""
#  *3    18-JAN-1991 17:21:05 CAMPBELL_T "Added dpabort.c"
#  *2    29-NOV-1990 16:59:17 BROGNA "Version 2.21"
#  *1     5-NOV-1990 18:09:09 GRABEL "initial..."
#  DEC/CMS REPLACEMENT HISTORY, Element SCSAMPLE.MS
# SCSI Toolkit -- Demo Sample Make file
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

TKLIB = ..\tkscsi\tkscsi.lib
OBJS = demosup.obj sleep.obj

demosup.obj  : demosup.c demosup.h ..\tkscsi\dpalette.h
   cl -c -A$(C_model) -I..\tkscsi demosup.c ;

sleep.obj    : sleep.c
   cl -c -A$(C_model) sleep.c ;

apic.obj     : apic.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi apic.c ;

sapic.exe     : apic.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 apic.obj $(OBJS), sapic, /map, $(TKLIB) ;

bdparm.obj   : bdparm.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi bdparm.c ;

sbdparm.exe   : bdparm.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 bdparm.obj $(OBJS), sbdparm, /map, $(TKLIB) ;

timeit.obj   : timeit.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi timeit.c ;

stimeit.exe   : timeit.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 timeit.obj $(OBJS), stimeit, /map, $(TKLIB) ;

tkstat.obj   : tkstat.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi tkstat.c ;

stkstat.exe   : tkstat.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 tkstat.obj $(OBJS), stkstat, /map, $(TKLIB) ;

tkfilm.obj   : tkfilm.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi tkfilm.c ;

stkfilm.exe   : tkfilm.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 tkfilm.obj $(OBJS), stkfilm, /map, $(TKLIB) ;

npics.obj    : npics.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi npics.c ;

snpics.exe    : npics.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 npics.obj $(OBJS), snpics, /map, $(TKLIB) ;

aspect.obj   : aspect.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi aspect.c ;

saspect.exe   : aspect.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 aspect.obj $(OBJS), saspect, /map, $(TKLIB) ;

mosaic.obj   : mosaic.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi mosaic.c ;

smosaic.exe   : mosaic.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 mosaic.obj $(OBJS), smosaic, /map, $(TKLIB) ;

backg.obj    : backg.c demosup.obj sleep.obj ..\tkscsi\dpalette.h demosup.h
   cl -c -A$(C_model) -I..\tkscsi backg.c ;

sbackg.exe    : backg.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 backg.obj $(OBJS), sbackg, /map, $(TKLIB) ;

dpabort.obj    : dpabort.c ..\tkscsi\dpalette.h
   cl -c -A$(C_model) -I..\tkscsi dpabort.obj

sdpabort.exe    : dpabort.obj $(OBJS) $(TKLIB)
   link /EXEpack /NOE /STACK:8096 dpabort.obj $(OBJS), sdpabort, /map, $(TKLIB) ;

