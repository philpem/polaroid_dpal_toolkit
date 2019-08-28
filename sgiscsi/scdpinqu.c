/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPINQU.C */
/*  *5     3-MAY-1991 12:12:31 BROGNA "" */
/*  *4     1-APR-1991 18:03:55 BROGNA "Toolkit enhancments" */
/*  *3    18-JAN-1991 22:48:17 GRABEL "changes to run under LSC on the MAC" */
/*  *2    18-JAN-1991 04:57:12 BROGNA "MSC 6.0x" */
/*  *1     5-NOV-1990 18:07:26 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPINQU.C */
/*  scdpinqu.c
 Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
     
20-sep-90  vg       initial, started with dpinquir.c from the Centronics
                    toolkit
 2-nov-90  vg       removed unused local variables
18-jan-91  vg       mb added a test for LSCV3.  An include of tkhost.h must
                      preceed this
*/
#include "tkhost.h"

#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif

#define DPINQUIR_C
#include "dpalette.h"
#include "tksccmds.h"
#include "scdperrs.h"

/* prototypes for local functions */
int DP_GetPrinterStatus(DP_DATA *, int) ;
int DP_ConfirmBuffer(DP_DATA *, int) ;
int DP_ExposureWarning(DP_DATA *) ;
void DP_sleep(int);

/*
  DP_GetPrinterStatus(dp, Sflag)
        Send exposure parameters to Digital Palette.
        Input:   dp - Pointer to structure of type DP_DATA.
                 Sflag  - Flag indicating which Inquiries to make.
*/


int DP_GetPrinterStatus( dp, Sflag )
DP_DATA   *dp ;
int        Sflag ;
{
    
    int   i,j;
    
    
    /* a local table is used to read the current status data and then the
       mode sense data.  The mode sense buffer is larger so this is used
       to set the size of the local_table
    */
    unsigned char  local_table[ SENSE_BUFFER_SIZE ] ;
    
    if (Sflag & INQ_ERROR)
    {    /* Inquire Error    */

        /* A HARDWARE or LOGIC error got us here so we want to clear the error
           and then ask the Digital Palette for an error description that we
           will use for an error message.
        
           For the SCSI interface, the current error may already have 
           been reported to the host. If there's an error currently
           in the status structure, then there's nothing to do. Otherwise,
           issue a Test Unit Ready command to see if any errors are pending
           in the Palette.
        */
         
        if ( !DP_ERRORCLASS(dp) )
            DP_ERRORCLASS(dp) = sc_test_unit_ready() ;

        /* If a TIMEOUT error got us here then there is no point in further
           attempts at communication with the Digital Palette.  Set an error
           message that the user can understand and return the error 
           condition.
        */
        if (DP_ERRORCLASS(dp) == TIMEOUT)
        {
            DP_DPALETTE_ONLINE(dp) = 0 ;
            DP_DPALETTE_INITIALIZED(dp) = 0 ;
        }
        
        /* Some toolkit routines which send buffered commands and/or
           data to the Digital Palette will check available buffer space 
           before sending.  If there is insufficient buffer space, and
           DP_WAITFORBUFFER is set to SHOW_BUFFER_WARNING, then the 
           routine will set the errorclass to BUFFERWARNING. For error 
           handling consistency the return will occur via a call to this
           routine.
           In the spirit of error handling we will set an error message that
           the user can understand and return the error condition.
        */
        else if (DP_ERRORCLASS(dp) == BUFFERWARNING)
            strcpy(DP_ERRORMESSAGE(dp), NoBufferSpace) ;
        
        else if (DP_ERRORCLASS(dp) == EXPOSUREACTIVE)
            strcpy(DP_ERRORMESSAGE(dp), ExposureInBox) ;
        else if (DP_ERRORCLASS(dp) == REMOVEFILM)
            strcpy(DP_ERRORMESSAGE(dp), ExposureComplete) ;
        else if (DP_ERRORCLASS(dp) == NOERROR)
        {   /*  No error set at this point.  */
            DP_ERRORNUMBER( dp ) = 0 ;
            strcpy(DP_ERRORMESSAGE(dp), NoErrorsReported) ;
        }

    }

    if (Sflag & (INQ_STATUS | INQ_BUFFER ))
    {    /* Inquire Status and Inquire Buffer  */

        /*  All the data which is needed to supply the toolkit status is
            provided by the DFR SCSI command, CURRENT STATUS.  Read the
            CURRENT STATUS and move the data into the status structure.
            Requests for Status and for Buffer information can be handled
            with the same SCSI command here, so we'll just group them
            together.
        */
        if (DP_ERRORCLASS(dp) = sc_curr_status( local_table ))
            return(DP_GetPrinterStatus(dp, INQ_ERROR)) ;

        /* good read, fill in the status structure with what we got */
        DP_STATUS_BYTE_ZERO( dp ) = 0x80 ;
        if (local_table[0] || local_table[1])
            DP_STATUS_BYTE_ZERO( dp )  |= 1 ; /* buffer space available */
        if ((local_table[5] < 3) || ( local_table[4] ) )
            DP_STATUS_BYTE_ZERO( dp )  |= 2 ;    /* we're exposing */
        if (local_table[6])
            DP_STATUS_BYTE_ZERO( dp )  |= 4 ;    /* single image mode */
        
        /* ucStatusByte1 contains the current color in the upper bits and
           the number of queued images in the lower 5 bits
        */
        DP_STATUS_BYTE_ONE(dp) = (char) ( 0x80 | (local_table[5] << 5) & 0x60)
                                      | (char) (local_table[4] & 0x1F) ;
        DP_LINES_EXPOSED( dp ) = (int) ((local_table[2] << 8) & 0xFF00 ) |
                                     (int)(local_table[3] & 0xFF ) ;
        
        DP_FREE_BUFFER( dp ) = ((local_table[0] << 8) & 0xFF00)
                                 |  (local_table[1] & 0xFF ) ;

    }

    if (Sflag & INQ_OPTIONS)
    {   /* Inquire Options */

        /*  Options are returned by the Inquire command which was
            performed during the init function.  There's no need to do another
            Inquire command, since the data returned in this command is
            configuration data and never changes.  During the init call,
            all of the appropriate data was placed in the DP_DATA structure.
            There's nothing to do....
        */
    
             
    }

    if (Sflag & INQ_CAMERA)
    {    /* Inquire Camera */

        /*  The current camera back data is returned as part of the 
            mode sense command.  We'll have to issue this command since
            there is no guarantee that the camera back hasn't been changed
            since the last mode sense was issued.
                
        */
        if (DP_ERRORCLASS(dp) = sc_mode_sense( local_table ))
            return(DP_GetPrinterStatus(dp, INQ_ERROR)) ;

        /* set the msbit of the camera code to be compatible with
           data format in the dp structure
        */
        DP_CAMERA_CODE( dp ) = (char)
                      (local_table[ MS_CURRENT_CAMERA_BACK ] | 0x80) ;

        strcpy(DP_CAMERADESCRIPTION( dp ),
                &local_table[ MS_CURRENT_CAMERA_BACK + 1 ] ) ;
                

    }

    if (Sflag & INQ_FILMS)
    {    /* Inquire Films  */

        /*
         Send the "Inquire Film Name" command to the Digital Palette.
         DPsend() will use the "Send Message" command to get the
         Digital Palette Film Name message.
        */
        for (i = 0 ; i < MAX_NUM_FILMS ; i++)
        {
#ifdef DEBUG2
	    printf("inquire film %d: %s\n", i, DP_FILM_NAME(dp,i));
#endif
            DP_ERRORCLASS(dp) = sc_film_name( i, DP_FILM_NAME(dp,i));
                    
		    /* MK Hack */
		    
		    DP_ERRORCLASS(dp) = NOERROR ;

            if (DP_ERRORCLASS(dp))
            {
                if ((DP_ERRORCLASS(dp) == LOGICERR) &&
                        (DP_ERRORNUMBER( dp ) == SCS_FILMNO_OUT_OF_RANGE ))
                {
                    /* not a real error, this film slot is empty */
                    dp->saFilmName[i][0] = '\0' ;
                    DP_ERRORCLASS(dp) = NOERROR ;
                }
                else
                {
                    DP_GetPrinterStatus(dp, INQ_ERROR) ;
                    return(DP_ERRORCLASS(dp)) ;
                }
            }
        }
        
        /* no real errors so far, fill in the error status entries with
           a call to inquire errors
        */
        DP_GetPrinterStatus(dp, INQ_ERROR) ;        
    }
      
    if (Sflag & INQ_ASPECT)
    {    /* Inquire Aspect Ratio */
    
        /*
          Send the "Inquire Aspect Ratio" command to the Digital
          Palette.
        */
        for ( i = 0 ; i < MAX_NUM_FILMS ; i++)
        {
            if (DP_FILM_PRESENT( dp, i ))
            {
#ifdef DEBUG
		printf("Inquire a/r of film %d.\n", i);
#endif
                if (DP_ERRORCLASS(dp) = sc_film_aspect( i,
                                                   dp->caAspectRatio[i] ))
		{
		  DP_ERRORCLASS(dp) = NOERROR ;
		  continue;
                  return(DP_GetPrinterStatus(dp, INQ_ERROR)) ;
		}
            }
            else
            {
#ifdef DEBUG
		printf("Do not inquire a/r of film %d.\n", i);
#endif
                dp->caAspectRatio[i][0] = 0 ;
                dp->caAspectRatio[i][1] = 0 ;
            }
            
            /* the third byte is always a null */
            dp->caAspectRatio[i][2] = 0 ;
        }
    }

    return(DP_ERRORCLASS(dp)) ;
}


/* DP_ConfirmBuffer */
int DP_ConfirmBuffer(dp, BytesNeeded)
DP_DATA  *dp ;
int           BytesNeeded ;
{
    int           Kbytes ;

    /*
    * Convert to KiloBytes which is how we'll get the buffer availabilty from
    * the Digital Palette.
    */
    Kbytes = BytesNeeded >> 10 ;    /* quick divide by 1024 */
    if (BytesNeeded & 0x3FF) ;      /* quick modulo 1024 test */
        Kbytes++ ;

    /*
       Make sure that the Digital Palette has Kbytes in available buffer
       space. Adjust the BufferFree entry in the Status structure.  When
       this value gets below 4K, then do a real inquire buffer to refresh
       the available buffer value from the Palette.  If the real value
       gets down to 4K, then we'll check to see if the app wants to 
       WaitForBuffer or get a Buffer warning

       If the WaitForBuffer flag is not set then the caller wants to know
       about the lack of buffer space and we'll return BUFFERWARNING.
    */

    if ( Kbytes < DP_FREE_BUFFER( dp ) )
    {   /* our BufferFree value says there's plenty of room */
        DP_FREE_BUFFER( dp ) -= Kbytes ;    /* adjust our copy */
        return( 0 ) ;                           /* no errors, all set */
    }

    
    /* wait for the required amount of buffer space to become available.
       To keep from consumming host and palette compute time. Sleep
       for a second between tests.
    */
    do
    {
        /* inquire the Palette to get the amount of buffer available */
        if (DP_GetPrinterStatus(dp, INQ_BUFFER))
            return(DP_ERRORCLASS(dp)) ;
        
        if (DP_FREE_BUFFER( dp ) < Kbytes )
        {
            if ( !DP_WAITFORBUFFER( dp ) )
            {
                DP_ERRORCLASS(dp) = BUFFERWARNING ;
                return(DP_ERRORCLASS(dp)) ;
            }
            DP_sleep( 1 ) ;
        }
     } while ( DP_FREE_BUFFER( dp ) < Kbytes ) ;

     return(DP_ERRORCLASS(dp)) ;
}


/* DP_ExposureWarning */
int  DP_ExposureWarning( dp )
DP_DATA   *dp  ;
{
    /* Ask the Digital Palette is it is currently exposing an image.  If it
       isn't then this routine doesn't need to generate a warning. Otherwise
       a warning is returned.
    */
    if (DP_GetPrinterStatus( dp, INQ_STATUS))
        return(DP_ERRORCLASS( dp )) ;
    
    if (!DP_EXPOSINGIMAGE( dp ))
        return(DP_ERRORCLASS( dp )) ;
    
    if (!DP_SINGLEIMAGEMODE( dp ))
        return(DP_ERRORCLASS( dp )) ;

    /* If the WaitForBuffer flag is not set and an exposure is in progress and
       and the camera is not an auto advance type then we'll return an
       EXPOSUREACTIVE warning immediately.
    */
    if (!DP_WAITFORBUFFER( dp ))
    {
        DP_ERRORCLASS( dp ) = EXPOSUREACTIVE ;
        return(DP_ERRORCLASS( dp )) ;
    }
   
    /* Keep monitoring the progress of the exposure until it is complete
       To keep from consuming the compute time of the host and the palette
       check in on a 1 second interval.
    */
    while (DP_EXPOSINGIMAGE( dp ))
    {
        if (DP_GetPrinterStatus( dp, INQ_STATUS))
            return(DP_ERRORCLASS( dp )) ;
        DP_sleep( 1 ) ;
    }

    /* Exposure is complete.  Set a warning to remove the film and return */
    DP_ERRORCLASS( dp ) = REMOVEFILM ;   
    return(DP_ERRORCLASS( dp )) ;
}

void DP_sleep(secs)
int secs;
{
  DP_Sleep(secs);
}


