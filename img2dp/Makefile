TIFF = /indy2/mk/tiff/tiff-v3.4beta028/libtiff
FORMS = /indy2/mk/forms/FORMS

CFLAGS = -O2 -I$(TIFF) -I/indy2/mk/polaroid/sgiscsi -I$(FORMS) -cckr -DDEBUG
LFLAGS = -O2
LLIBS1 = -L$(TIFF) -ltiff -L/indy2/mk/polaroid/sgiscsi -ltkscsi -L/sgi320/4Dgifts/iristools/libimage -limage -lds
LLIBS2 = -L$(FORMS) -lforms -L/indy2/mk/polaroid/sgiscsi -ltkscsi

all:	/nmr5/bin/img2dp /nmr5/bin/dpc bwtest

/nmr5/bin/img2dp:	img2dp.o $(TIFF)/libtiff.a /indy2/mk/polaroid/sgiscsi/libtkscsi.a
	cc $(LFLAGS) -o /nmr5/bin/img2dp img2dp.o $(LLIBS1) -lm -lmalloc -lc_s

img2dp.o:	img2dp.c $(TIFF)/tiffio.h /indy2/mk/polaroid/sgiscsi/dpalette.h
	cc $(CFLAGS) -c img2dp.c

bwtest:	bwtest.o $(TIFF)/libtiff.a
	cc $(LFLAGS) -o bwtest bwtest.o $(LLIBS1) -lm -lmalloc -lc_s

bwtest.o:	bwtest.c $(TIFF)/tiffio.h
	cc $(CFLAGS) -c bwtest.c

/nmr5/bin/dpc:	dpc.o dpcform.o
	cc $(LFLAGS) -o /nmr5/bin/dpc dpc.o dpcform.o $(LLIBS2) -lfm_s -lgl_s -lm -lmalloc -lc_s

dpc.o:	dpc.c dpcform.h
	cc $(CFLAGS) -c dpc.c

dpcform.o:	dpcform.c
	cc $(CFLAGS) -c dpcform.c

