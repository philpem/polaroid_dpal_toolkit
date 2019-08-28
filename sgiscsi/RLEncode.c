/*  DEC/CMS REPLACEMENT HISTORY, Element RLENCODE.C */
/*  *6     3-MAY-1991 12:11:49 BROGNA "" */
/*  *5     1-APR-1991 18:03:33 BROGNA "Toolkit enhancments" */
/*  *4    18-JAN-1991 04:56:57 BROGNA "MSC 6.0x" */
/*  *3     5-NOV-1990 17:37:47 GRABEL "added conditional include" */
/*  *2     3-OCT-1990 15:15:20 BROGNA "Single data structure" */
/*  *1    12-SEP-1990 11:03:55 BROGNA "ToolKit Upgrade" */
/*  DEC/CMS REPLACEMENT HISTORY, Element RLENCODE.C */
/* RGBtoRLE.C
 * Modified from TrueVision Targa Toolkit RLPutPic.c algorithm.
 */

#include <stdio.h>
#include "tkhost.h"

#ifdef LSCV3        /* LightSpeed C on the Macintosh, non-ANSI */
#include <strings.h>
#else
#include <string.h>
#endif

#define RGBTORLE_C
unsigned int RLEncode(char *, char *, int) ;

/*
 * RLEncode() Run Length Compression of image data.
 *
 *   Usage:
 *         RGBtoRLE(in, out, NumBytes)
 *            char *in ;        Buffer to compress
 *            char *out ;       Buffer to write it out to
 *            int   NumBytes ;  Number of bytes input
 */
unsigned int
RLEncode(in, out, NumBytes)
   char *in ;
   char *out ;
   int   NumBytes ;
   {
   int   PixelCount ;      /* Number of pixels in the current packet */
   char *CurrentPixel ;    /* current input pixel position */
   char *ComparePixel ;    /* pixel value to match for a run */
   char *PacketByte ;      /* location of last flag byte to set */
   int   PacketLength ;    /* current count in R.L. String */
   char *FirstPacketByte ; /* location to begin copying at */
   int i;

   /*
    * Set up to go
    */
#ifdef DEBUG2
   printf("RLEncode: bytes: %d, from: %d, to: %d\n", NumBytes, in, out);
#endif
   PacketByte = &out[0] ;  /* place to put next flag if run */
   CurrentPixel = &in[0] ;
   PixelCount = 1 ;
   PacketLength = 0 ;
   FirstPacketByte = (char *)0 ;

   /*
    * Loop until we've gotten all the input bytes.
    */
   do 
      {
      /*
       * if we have accumulated a 128-byte packet, process it
       */
      if (PixelCount == 129)
         {
         *PacketByte = 127 ;
         if (PacketLength >= 2)
            {
            *PacketByte |= 0x80 ;
            PixelCount = 2 ;
            }
         ++PacketByte ;
#ifdef DEBUG2
	 printf("line: %d, move %d bytes.\n", __LINE__, (PixelCount - 1));
#endif
         /* memmove(PacketByte, FirstPacketByte, PixelCount-1) ; */
	 for(i=0; i<PixelCount-1; i++) PacketByte[i] = FirstPacketByte[i];
         PacketByte += PixelCount-1 ;
         PixelCount = 1 ;
         continue ;
         }
      
      /*
       * Treat byte zero specially.
       */
      if (PixelCount == 1)
         {
         PacketLength = 0 ;
         FirstPacketByte = CurrentPixel ;
         ComparePixel = CurrentPixel ;
         PixelCount = 2 ;
         CurrentPixel ++ ;
         continue ;
         }

      /*
       * build a packet as either raw data or encoded.
       */
      if (*CurrentPixel == *ComparePixel)
         {
         /*
          * make sure that saving a packet of encoded data takes less bytes
          * than saving a packet of raw data.
          */
         if (++PacketLength == 2)
            {
            /*
             * terminate the previous raw data packet
             */
            if (PixelCount > (PacketLength+1))
               {
               *PacketByte++ = (char) (PixelCount - 4) ;
#ifdef DEBUG2
	       printf("To: %ld, From: %ld\n", PacketByte, FirstPacketByte);
	       printf("line: %d, move %d bytes.\n", __LINE__, (PixelCount - 1 - PacketLength));
#endif
	       for(i=0; i<(PixelCount-1-PacketLength); i++) PacketByte[i] = FirstPacketByte[i];
               /* memmove(PacketByte, FirstPacketByte, (PixelCount - 1 - PacketLength)) ; */
               PacketByte += (PixelCount - 1 - PacketLength) ;
               FirstPacketByte = CurrentPixel ;
               PixelCount = PacketLength + 1 ;
               }
            }
         }
      else
         {
         /*
          * no match.  if the current packet is encoded, terminate it.  other-
          * wise continue on with a raw data packet.
          */
         if (PacketLength >= 2)
            {
            *PacketByte++ = (char)(0x80 | PacketLength) ;
#ifdef DEBUG2
	     printf("line: %d, move %d bytes.\n", __LINE__, 1);
#endif
	    PacketByte[0] = FirstPacketByte[0];
            /* memmove(PacketByte, FirstPacketByte, 1) ; */
            PacketByte ++ ;
            PixelCount = 1 ;
            continue ;
            }
         else
            {
            PacketLength = 0 ;
            ComparePixel = CurrentPixel ;
            }
         }
      PixelCount++ ;
      CurrentPixel ++ ;
      } while (CurrentPixel < (in + NumBytes)) ;

   /*
    * terminate any unfinished packets so we don't lose data.
    */
   if (--PixelCount >= 1)
      {
      *PacketByte = (char)(PixelCount - 1) ;
      if (PacketLength >= 2)
         {
         *PacketByte |= 0x80 ;
         PixelCount = 1 ;
         }
      ++PacketByte ;
#ifdef DEBUG2
      printf("line: %d, move %d bytes.\n", __LINE__, PixelCount);
#endif
      for(i=0; i<PixelCount; i++) PacketByte[i] = FirstPacketByte[i];
      /* memmove(PacketByte, FirstPacketByte, PixelCount) ; */
      PacketByte += PixelCount ;
      }

   /*
    * return the encoded buffer size.
    */
   return(PacketByte - out) ;
   }
