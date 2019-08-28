/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPSCMD.C */
/*  *6     8-MAY-1991 19:39:36 GRABEL "added id check" */
/*  *5     3-MAY-1991 12:13:26 BROGNA "" */
/*  *4    20-FEB-1991 08:55:48 CAMPBELL_T "mods to DP_scsi_init for multiple printers" */
/*  *3    18-JAN-1991 22:48:28 GRABEL "changes to run under LSC on the MAC" */
/*  *2    18-JAN-1991 04:57:34 BROGNA "MSC 6.0x" */
/*  *1     5-NOV-1990 18:08:25 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPSCMD.C */
/*  tksccmds.c

    Contains mid-level functions which use the host specific SCSI interface.

DP_doscsi_cmd()         generic function called to execute most of the scsi
                        commands.
                        
DP_scsi_read_forfilm()  called for the inquire film and inquire aspect ratio
                        commands.

dpscsi_init()           Finds the Digital Palette on the SCSI bus and issues
                        an Inquiry command
                        
                        

25-sep-90  vg       initial....
 2-nov-90  vg       removed unused local variables
18-jan-91  vg       include tkhost.h must preceed LSCV3 conditional test
30-jan-91  vg       mods to DP_scsi_init to accomodate multiple printers
                    on the SCSI bus
17-apr-91  vg       added a notion of what scsi id's filmprinters were found
                     at during an init.  During a normal call, the scsi_id
                     is verified to make sure we're not going to talk to
                     some other scsi device
                         
*/

#include <stdlib.h>
#include <stdio.h>

#include "tkhost.h"

#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif

#include "dpalette.h"

#define SCDPSCMD_C
#include "tksccmds.h"
#include "scdperrs.h"

int DP_doscsi_cmd(DP_DATA *, int, unsigned char *, int, int, int, int) ;
int DP_scsi_read_forfilm(DP_DATA *, int, char *, int, int, int) ;
int DP_scsi_init(DP_DATA *) ;
int DP_firmware_rev(DP_DATA *) ;


/* A static array is maintained here which has 1 entry for each of the
   eight possible scsi id's.  All of these values are initially set to
   NOT_ACTIVE.  When the init function, DP_scsi_init(), is called and
   a filmprinter is found, the appropriate entry in this array is set
   to ACTIVE.  The DP_doscsi_cmd function will verify that the scsi id
   that we're about to talk to is an active filmprinter before the
   low level driver function is called.  This provides an added measure
   of protection so that we don't try to talk to a scsi device which is
   not a filmprinter.
   If the scsi id which is passed is NOT_ACTIVE, then a hardware error 
   is returned.
*/
#define NOT_ACTIVE 0
#define ACTIVE     1

static char scsiid_state[8] =
{
    NOT_ACTIVE, NOT_ACTIVE,
    NOT_ACTIVE, NOT_ACTIVE,
    NOT_ACTIVE, NOT_ACTIVE,
    NOT_ACTIVE, NOT_ACTIVE
} ;

#define IS_SCSI_NOT_ACTIVE(p)   ( ( p < 0 ) || ( p > 7 ) || \
                                ( scsiid_state[ p ] == NOT_ACTIVE ))




/* DP_doscsi_cmd is called to execute most of the scsi commands. (the 
   only exceptions are the two scsi functions dealing with film- see
   DP_scsi_read_forfilm() below).  This function will construct a CDB and
   call the host specific low level scsi function to perform the command.
   On return, if an error occurs, the error reporting function is called
   to fill the DP_DATA structure with the error info.
*/
int
DP_doscsi_cmd( dp, command_name, data_ptr, data_size, command_arg, color, dir)
DP_DATA *dp ;
int command_name, data_size, command_arg, color, dir ;
unsigned char *data_ptr ;
{
    int status, port ;
    char extended_sense[ SENSE_SIZE ] ;
    char cdb[CDB_SIZE] ;

    /* make sure the dp_port value is for an inited filmprinter */
    port = DP_PORT( dp ) - DP_SCSIID_OFFSET ;
    if ( IS_SCSI_NOT_ACTIVE( port ) )
    {
        DP_ERRORCLASS(dp) = HARDWERR ;
        DP_ERRORNUMBER( dp ) = SC_UNKNOWNPORT ;
        strcpy(DP_ERRORMESSAGE(dp), Incommunicado ) ;
        return( DP_ERRORCLASS( dp )) ;
    }
        
    
    /* construct a CDB for this SCSI command */
    cdb[0] = (char)command_name ;
    cdb[1] = 0 ;
    cdb[2] = (char)command_arg ;
    cdb[3] = (char)(data_size >> 8) ;
    cdb[4] = (char)(data_size & 0xFF) ;
    cdb[5] = (char)( color << 6 ) ;
    
    status = DP_scsi_driver( cdb, port,
                             (char *)data_ptr, data_size, 
                             extended_sense, dir ) ;
    
    if (status != SCSI_COMMAND_OK )
        return( DP_retrieve_scsi_error( dp, status, extended_sense )) ;
    
    return( status ) ;

}

/* DP_scsi_read_forfilm is a special call to deal with the dfr subcommands
   inquire film and inquire aspect ratio.  These two commands are deviants
   since they place the film number in cdb byte 3 where the data size MSB
   would normally go.
*/       
int DP_scsi_read_forfilm( dp, command_name, data_ptr, data_size, 
                      command_arg, filmno)
DP_DATA *dp ;
int command_name, data_size, command_arg, filmno;
char *data_ptr ;
{
    int status, port ;
    char extended_sense[ SENSE_SIZE ] ;
    char cdb[CDB_SIZE] ;
    
    /* make sure the dp_port value is for an inited filmprinter */
    port = DP_PORT( dp ) - DP_SCSIID_OFFSET ;
    if ( ( port < 0 ) || 
         ( port > 7 ) ||
         ( scsiid_state[ port ] == NOT_ACTIVE ))
    {
        DP_ERRORCLASS(dp) = HARDWERR ;
        DP_ERRORNUMBER( dp ) = SC_UNKNOWNPORT ;
        strcpy(DP_ERRORMESSAGE(dp), Incommunicado ) ;
        return( DP_ERRORCLASS( dp )) ;
    }
        
    /* construct a proper CDB */
    cdb[0] = (char)command_name ;
    cdb[1] = 0 ;
    cdb[2] = (char)command_arg ;
    cdb[3] = (char)filmno ;
    cdb[4] = (char)(data_size % 256) ;
    cdb[5] = 0 ;    /* no color bits for these calls */
    
    status = DP_scsi_driver( cdb, DP_PORT( dp ) - DP_SCSIID_OFFSET,
                             data_ptr, data_size, 
                             extended_sense, SCSI_READ ) ;
    
    if (status != SCSI_COMMAND_OK )
        return( DP_retrieve_scsi_error( dp, status, extended_sense )) ;
    
    return( status ) ;

}


/* DP_scsi_init() is called once to initialize the scsi port.  It will
   search all of the scsi id's, looking for the Palette.  For every
   device which responds to selection, an inquiry command is issued and
   the identification string is examined.  If a Palette is found, the
   scsi id for the Palette is saved in DP_PORT(dp) and all of the data
   in the DP_DATA structure which is available from the Inquiry command
   is filled in.
25-sep-90  vg       initial....
30-jan-91  vg       mods to accomodate multiple printers.
                    1) The scsi id's are searched until the id gets back
                       to INITIAL_SCSI_ID.
                    2) Added sc_short_inquire() call for speed
17-apr-91  vg       Added protection from talking to a non filmprinter scsi
                     device with the scsiid_state array
*/

/* identification string returned by a palette in the inquiry command */
#define IDENT_STRING "DP2SCSI"

/* scsi id where we start looking for the palette */
#define INITIAL_SCSI_ID 1   

int DP_scsi_init( dp )
DP_DATA *dp ;
{
    int X, status, first_scsi_id, max_size ;
    char extended_sense[ SENSE_SIZE ] ;
    unsigned char inq_buffer[INQUIRY_BUFFER_SIZE] ;

    
    /* Get the DP_PORT value from the DP_DATA structure.  If it is in
       the range of a valid SCSI ID, then begin the search for a Palette
       with this value.
       
       This way, if multiple calls to init are made, we'll
       return faster because the subsequent init calls will be checking
       the correct id first.
           
       This also provides the app with a means of initing more than one
       Palette which may be connected to the SCSI bus.
    */
    
    first_scsi_id = DP_PORT( dp ) ;
    
    if ( first_scsi_id >= DP_SCSIID_OFFSET )
    {
        first_scsi_id -= DP_SCSIID_OFFSET ;
        if ( first_scsi_id > 7 )
        {
            /* scsi id out of range, return an error */
            DP_ERRORCLASS(dp) = UNKNOWNPORT ;
            DP_ERRORNUMBER( dp ) = SC_UNKNOWNPORT ;
            strcpy(DP_ERRORMESSAGE(dp), BadPortNumber ) ;
            return( DP_ERRORCLASS( dp )) ;
        }
    }
    else
        first_scsi_id = INITIAL_SCSI_ID ;
                                /* begin our search with INITIAL_SCSI_ID If
                                   this value is set to 1, it makes
                                   for a fast find if the palette
                                   is at the factory setting
                                */
    
    X = first_scsi_id ;
    
    do
    {

        DP_PORT( dp ) = X + DP_SCSIID_OFFSET ;
        
        /* a short inquiry command will do a SCSI INQUIRY and only read the
           data which is REQUIRED by the SCSI spec.  If a full inquiry command
           is issued to a scsi device other than the Palette, the other device
           may not return as much data as a Palette.  If this happens, then
           the low level scsi driver will get a data_in phase timeout. 
           There's no problem with this except that the code hangs for the
           timeout period which could be many seconds.  This is simply a
           technique to make the init faster.
               
           For this init call, since there may not be any device out there,
           we'll temporarily set the scsiid_state entry to ACTIVE.
        */
        scsiid_state[X] = ACTIVE ;
        status = sc_short_inquire( inq_buffer ) ;
        scsiid_state[X] = NOT_ACTIVE ;    /* reset the state */
        
        if ( status == SCSI_COMMAND_OK )
        {
            /* a scsi device responded properly, look at the identification
               and see if it matches what we expect from a palette
            */
            if ( !( strcmp( &inq_buffer[ INQ_IDENTIFICATION ],
                                         IDENT_STRING )))
            {
                /* set the entry for this scsi id to active in the 
                   scsiid_state array
                */
                scsiid_state[X] = ACTIVE ;
                
                /* talking to a Palette, get all of the inquire data */
                status = sc_inquire( inq_buffer ) ;
                
                /* oops, error getting full inquire data */
                if ( status != SCSI_COMMAND_OK )
                {
                    scsiid_state[X] = ACTIVE ;
                    break ;
                }
                
                /* The scsi id which is in the DP_DATA structure is the
                   correct id for the palette.

                   fill in those items in the DP_DATA struct which are
                   returned by the inquiry command.  All data returned
                   by the inquiry command is non-volitile, ie. it's static
                   data which doesn't change.  As long as the application
                   doesn't change these values ( which is assumed ), then
                   we need only set them at init time.
                */
                
                /* build the options message, it goes something like this:
                   VERSION XXX, 4K pixels
                
                   XXX is filled in with the three character, null terminated 
                   ascii string which is in the inquire buffer as the revision
                   level.  The '4' in the example above is either 2 or 4
                   depending on the maximum resolution.  This value is a
                   binary value which currently resides in the local variable,
                   max_size.  Use sprintf to move this data to the
                   DP_DATA structure.
               
                */
                
                if ( inq_buffer[ INQ_HRES_MAX_MSB ] == 4096 / 256 )
                    max_size = 4 ;
                else
                    max_size = 2 ;
        

                sprintf( DP_ROM_VERSION( dp ), "VERSION %s, %1dKpixels",
                    &inq_buffer[ INQ_REVISION_LEVEL ], max_size ) ;
                
                /* palette firmware bug...
                   In version 170 of the firmware, the maximum buffer size
                   which is returned is actually the total amount of ram.
                   If the firmware rev is version 170, then adjust the
                   value which is returned.  The max buffer available is
                   62K less than the total ram available
                */
                   
                DP_TOTAL_BUFFER( dp ) =
                    ((inq_buffer[ INQ_MAX_BUFFER_SIZE_MSB ] << 8) & 0xFF00)
                   | (inq_buffer[ INQ_MAX_BUFFER_SIZE_LSB ] & 0xFF ) ;

                if ( DP_firmware_rev( dp ) == 170 )
                    /* palette version 170 firmware */
                    DP_TOTAL_BUFFER( dp ) = DP_TOTAL_BUFFER( dp ) - 62 ;
                    
                DP_OPTION_BYTE_ZERO( dp ) = 0x84 ;
        
                if ( max_size == 4 )
                    DP_OPTION_BYTE_ZERO( dp ) |= 1 ;
        
                /* get the total memory available in the palette. We'll
                   get this indirectly by looking at the max buffer size.
                   If it's greater than 128, then assume this is a 640K
                   machine
                */
                if ( DP_TOTAL_BUFFER( dp ) > 128 )
                    DP_OPTION_BYTE_ZERO( dp ) |= 2 ;
        
                /* indicate that no errors were found */
                DP_ERRORCLASS(dp) = NOERROR ;
                DP_ERRORNUMBER( dp ) = 0 ;
                strcpy(DP_ERRORMESSAGE(dp), NoErrorsReported) ;
                
                return( 0 ) ;       /* null says we found it */
            }
            
            DP_ERRORCLASS(dp) = HARDWERR ;
            DP_ERRORNUMBER( dp ) = SC_UNKNOWNPORT ;
            strcpy(DP_ERRORMESSAGE(dp), Incommunicado ) ;
                
        }
        
        else
            /* fill in the struct with the appropriate error */
            DP_retrieve_scsi_error( dp, status, extended_sense ) ;
            
        
        X = ++X & 7 ;   /* bump to next id ( modulo 7 ) */
        
    } while ( X != INITIAL_SCSI_ID ) ;
    
    /* return with an indication that the dp wasn't found */
    DP_PORT( dp ) = 0 ;
    return( DP_ERRORCLASS( dp ) ) ;
}


/* DP_firmware_rev returns the palette firmware revision as an integer
   value. 
*/       
int DP_firmware_rev( dp )
DP_DATA *dp ;
{
    char *rev_string ;
    
    rev_string = DP_ROM_VERSION( dp ) ;
    while ( *(rev_string++) != ' ' ) ;  /* move passed the first word  */
    
    return( atoi( rev_string ) ) ;      /* 2nd word is an ascii number */
                                        /* which is the revision       */
}
