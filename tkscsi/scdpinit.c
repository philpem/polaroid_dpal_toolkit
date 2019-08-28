/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPINIT.C */
/*  *7     3-MAY-1991 12:12:19 BROGNA "" */
/*  *6     1-APR-1991 18:04:29 BROGNA "Toolkit enhancments" */
/*  *5    25-MAR-1991 16:10:36 BROGNA "Bug Fix" */
/*  *4    18-JAN-1991 22:48:11 GRABEL "changes to run under LSC on the MAC" */
/*  *3    18-JAN-1991 04:57:04 BROGNA "MSC 6.0x" */
/*  *2     7-NOV-1990 14:45:44 BROGNA "Removed CONFIG_PARAM_BYTES..." */
/*  *1     5-NOV-1990 18:07:16 GRABEL "initial..." */
/*  DEC/CMS REPLACEMENT HISTORY, Element SCDPINIT.C */
/* scdpinit.c

 Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
      
20-sep-90  vg  initial....,  used dpinit.c from the Centronics toolkit
                             as a starting point
 4-oct-90  vg  modified for the new tk data structure
 2-nov-90  vg  removed unused local variables
 7-nov-90  mb  Checked unused variables for MSDOS. (path, drive)
	            Put definition of CONFIG_PARAM_BYTES in dpalette.h.
               Removed call to lseek().  (old code; incompatible with current
                  file i/o.)
18-jan-91  vg   test for LSCV3 must be after the include tkhost.h
18-jan-91  vg   made Pdata unsigned
                added an error message when the config file isn't found
28-mar-91  jb   substituted getenv()    with DP_GetConfigPath()
                substituted fopen()     with DP_FileOpen()
				substituted PathParse() with DP_PathParse()

 */

#include <stdio.h>          /* for unix strcat....                      */
#include "tkhost.h"
#ifdef LSCV3
#include <strings.h>
#else
#include <string.h>
#endif

#define DPINIT_C
#include "dpalette.h"

#include "tksccmds.h"       /* include toolkit scsi command definitions */
#include "scdperrs.h"       /* toolkit and dp errors                    */

#ifdef  MACINTOSH_OS
static char configfilename[] = "dpalette.cfg" ;
#endif


int DP_InitPrinter(DP_DATA *, int, int) ;


int DP_InitPrinter( dp, BufferWait, PortOverRide )
DP_DATA   *dp ;           /* pointer to toolkit data structure */
int       BufferWait ;    /* buffer availability warning suppression */
int       PortOverRide ;  /* overrides cfg port if not zero */
{
    int          SaveExpError = 0 ;
    int          FilmSize ;
    FILE         *fh ;
    int          Color, i, c ;
    char         *ptr ;
    unsigned char Pdata[4200] ;
    char          Escape = 0x1b ;
    char         *ConfigDir ;
    char          FilmFile[129] ;
    void          DP_PathParse() ;
    char         *scsi_name ;
#ifdef MSDOS
    char          Fname[9] ;
    char          Fext[5] ;
    char          Drive[3] ;
    char          Path[129] ;
#endif    
    
    DP_STATUS_BYTE_ZERO(dp) = 0 ;
    DP_STATUS_BYTE_ONE(dp) = 0 ;
    DP_LINES_EXPOSED(dp) = 0 ;
    DP_EXPOSURE_STATUS(dp)[0] = '\0' ;
    DP_OPTION_BYTE_ZERO(dp) = 0 ;
    DP_ROM_VERSION(dp)[0] = '\0' ;
    for (i = 0 ; i < MAX_NUM_FILMS ; i++)
        for (c = 0 ; c < ASPECT_RATIO_SIZE ; c++)
            DP_ASPECT_MESSAGE(dp, i)[0] = '\0' ;
    DP_FREE_BUFFER(dp) = 0 ;
    DP_TOTAL_BUFFER(dp) = 0 ;
    DP_CAMERA_CODE(dp) = 0 ;
    DP_CAMERADESCRIPTION(dp)[0] = '\0' ;
    DP_ERRORNUMBER(dp) = 0 ;
    DP_ERRORMESSAGE(dp)[0] = '\0' ;
    DP_ERRORCLASS(dp) = 0 ;
    DP_DPALETTE_INITIALIZED(dp) = 0 ;
    DP_DPALETTE_ONLINE(dp) = 0 ;

    strcpy(DP_TOOLKIT_VERSION( dp ), TOOLKIT_VERSION) ;

    /* init the host scsi driver, if an error occurs, punt */
    if ( DP_scsi_setup( dp, &scsi_name ))
        return( DP_ERRORCLASS( dp )) ;

    /* cat the scsi name onto the toolkit version */
    strcat( DP_TOOLKIT_VERSION( dp ), " " ) ;
    strcat( DP_TOOLKIT_VERSION( dp ), scsi_name ) ;

    /* Get the path/name of the configuration file from the environment
       variable DPALETTE.
    */
    
#ifdef MSDOS
    ConfigDir = DP_GetConfigPath() ;
#else
    ConfigDir = configfilename ;
#endif

    if (!ConfigDir)
    {
        DP_ERRORCLASS(dp) = CONFIGERR ;
        DP_ERRORNUMBER(dp) = CFG_NO_ENVIRONMENT ;
        strcpy( DP_ERRORMESSAGE(dp), "Config File not found" ) ;
        return(DP_GetPrinterStatus(dp, INQ_ERROR)) ;
    }

   
    /*  Parse the path and filename out of the environment string and open the
        file for read.
    */

#ifdef MSDOS
    DP_PathParse(ConfigDir, Drive, Path, Fname, Fext) ;
#endif

    if (DP_FileOpen(dp,ConfigDir))
    {
        DP_ERRORCLASS(dp) = CONFIGERR ;
        DP_ERRORNUMBER(dp) = CFG_FILE_OPEN_ERROR ;
        return(DP_GetPrinterStatus(dp, INQ_ERROR)) ;
    }

    /* Read the configuration data into the DP_DATA structure.  Make sure
       that all the data was read in.
    */
    FilmSize = DP_FileRead(dp, (char *) dp, CONFIG_PARAM_BYTES);
	DP_FileClose(dp) ;

    if (FilmSize < CONFIG_PARAM_BYTES)
    {
        DP_ERRORCLASS(dp) = CONFIGERR ;
        DP_ERRORNUMBER(dp) = CFG_FILE_READ_ERROR ;
        return( DP_ERRORCLASS( dp )) ;
    }

    /* Default Vertical Height is assumed to be centered in
       the vertical dimension  */
    DP_VER_HEIGHT(dp) = DP_VER_RES(dp) - (2 * DP_VER_OFFSET(dp)) ;
   
    /* ** Default Servo Method for scsi, default is servo off ** */
    DP_SERVO_MODE(dp) = DEFAULT_SERVO ;
   
    /* Set up a default Color table  */
    for (Color = 0 ; Color < 3 ; Color++)
    {
        for (i = 0 ; i < 256 ; i++)
            DP_COLOR_TABLE(dp, Color, i) = (char) i ;
    }

    /*  Default Background Mode = off,  Default Background Values  */
    DP_BACKGROUND_MODE(dp) = 0 ;
    DP_BACKGROUND_PIX_VALUE(dp) = 0 ;
    for (Color = 0 ; Color < 3 ; Color++)
    {
        DP_UPPER_LEFT_COLOR(  dp, Color) = 0 ;
        DP_UPPER_RIGHT_COLOR( dp, Color) = 0 ;
        DP_LOWER_LEFT_COLOR(  dp, Color) = 0 ;
        DP_LOWER_RIGHT_COLOR( dp, Color) = 0 ;
    }

    /* Default Image Compression */
    DP_IMAGE_COMPRESSION(dp) = NON_RLE_SEND_AS_RLE ;

    /* How do we want to handle a full buffer in the Digital Palette
       SHOW_BUFFER_WARNING:     Don't wait.  Return a Warning to the caller.
                                Let the caller decide whether to wait or not.
       SUPPRESS_BUFFER_WARNING: Wait until buffer space available
    */
    DP_WAITFORBUFFER(dp) = (char) BufferWait ;
   
    /* Set up and find the scsi id where the Palette is connected. 
       DP_scsi_init will go through all of the scsi id's to see if 
       any respond to an inquire command.  For devices which are found, 
       the inquire data is checked to see if the device is a palette.  
    */

    /* First, if the PortOverRide arg is non-zero, then use this value as
       the port number
    */
    if ( PortOverRide )     /* if non-zero, use it directly */
        DP_PORT( dp ) = PortOverRide ;
    
    if ( DP_scsi_init( dp ) )
        return(DP_ERRORCLASS(dp)) ;      /* palette not found, the error
                                            class, number, and msg in the
                                            dp structure indicate what 
                                            happened
                                         */

    DP_DPALETTE_ONLINE( dp ) = 1 ;       /* the filmprinter is online */

    /* Check for any existing errors or Power up Diagnostic errors in the
       Digital Palette.
       Up to this point, the only SCSI command which has been issued is
       the Inquire command ( issued in DP_dpscsi_init() ).  In SCSI, an
       Inquiry command will not wipe out any pending error conditions in
       the Palette.   The GetPrinterStatus call will retrieve any
       outstanding errors.

       There are a few errors which can be present at this time which the
       user should be alerted to.  Any other errors may be safely cleared
       and ignored since the next step in initializing the Digital Palette
       is to issue the Reset Command.  There are a few hardware errors
       (eg, out of film, just powered on) which are ignored.  All logical
       errors are also ignored.  Any other errors are reported immediately
       back to the caller.
    */
    DP_GetPrinterStatus( dp, INQ_ERROR) ;
    switch( DP_ERRORCLASS( dp ) )
    {
        case HARDWERR:
            switch( DP_ERRORNUMBER( dp ) )
            {
                /* we'll break through the following harware errors */
                case EOM_ERRNUMBER:
                case SCS_POWER_ON:
                    break ;
                    
                default:
                    return(DP_ERRORCLASS(dp)) ;
            }
            break ;
                    
        case TIMEOUT:
        case DISCONNECTED:
        case UNKNOWNPORT:
        case CONFIGERR:
        case DPTOOLKITERR:
            return( DP_ERRORCLASS(dp)) ;
            break ;
    }

   /*
    * See if we have to return any exposure warnings.
    */
    SaveExpError = DP_ExposureWarning( dp ) ;
    if ((!DP_WAITFORBUFFER( dp ) && DP_WARNING( dp )) || DP_ERROR( dp ))
        return(DP_GetPrinterStatus( dp, INQ_ERROR)) ;

    
    /* Reset the Digital Palette.  This clears any existing errors and puts
       the Palette in a machine default state.
    */
    if ((DP_ERRORCLASS(dp) = sc_reset_to_default()))
        return(DP_GetPrinterStatus(dp, INQ_ERROR)) ;

    /* Send all the Configuration File downloadable Data to the Digital
       Palette.

       Downloadable film tables and camera adjustments.
    */
    for (i = 0 ; i < MAX_NUM_FILMS ; i++)
    {
        if (DP_FILM_TABLE_FILE(dp, i)[0])
        {
#ifdef MSDOS
            strcpy(FilmFile, Drive) ;
            strcat(FilmFile, Path) ;
            strcat(FilmFile, dp->saFilmFile[i]) ;
#endif
#ifdef MACINTOSH_OS
            strcpy(FilmFile, dp->saFilmFile[i]) ;
#endif
            if (DP_FileOpen(dp, FilmFile))
            {
                DP_ERRORCLASS(dp) = CONFIGERR ;
                DP_ERRORNUMBER(dp) = CFG_FILM_OPEN_ERROR ;
                DP_GetPrinterStatus(dp, INQ_ERROR) ;
                strcat(DP_ERRORMESSAGE(dp), DP_FILM_TABLE_FILE(dp,i));
                return(DP_ERRORCLASS(dp)) ;
            }
            Pdata[0] = (char) i ;
            FilmSize = DP_FileRead(dp, &Pdata[1], MAX_FILM_FILE_SIZE) ;
			DP_FileClose(dp);
			            
            /* Make sure that the Digital Palette has sufficient buffer space
               for this film table.  If no errors or warnings at this
               point it is OK to send the film table to the Digital Palette.
            */
            if (!DP_ConfirmBuffer( dp,  FilmSize + DP_CMD_PACKET_SIZE))
            {
                if (DP_ERRORCLASS(dp) =
                            DP_DownLoadFilms( dp, Pdata, FilmSize + 1 ))
                    return(DP_GetPrinterStatus(dp, INQ_ERROR)) ;
            }
        }
    }
    /* Check on any known errors in the Digital Palette.
       Get the current Exposure status of the Digital Palette.
       Get the Digital Palette Options.
       Get the Digital Camera back information.
       Find out how much buffer space is available.
    */
    DP_GetPrinterStatus(dp, ALL_INQUIRIES) ;
    if (DP_ERRORCLASS(dp))
        return(DP_ERRORCLASS(dp)) ;

    /* Update the film printer.  If the Digital Palette reports a logic error,
       then we assume it is because of a bad parameter from the config file
       so we'll set Status->iErrorClass to reflect a configuration file
       error.  Status->sErrorMsg will still point to a Digital Palette message
       describing the error.
    */
    DP_SendPrinterParams(dp) ;
    if (DP_ERRORCLASS(dp) == LOGICERR)
        DP_ERRORCLASS(dp) == CONFIGERR ;

    /* Update the Error message  */
    if (!DP_ERRORCLASS( dp ))
        DP_ERRORCLASS( dp ) = SaveExpError ;
    DP_GetPrinterStatus(dp, INQ_ERROR) ;
   
    if (DP_ERRORCLASS(dp))
        DP_DPALETTE_INITIALIZED(dp) = 0 ;
    else
        DP_DPALETTE_INITIALIZED(dp) = 1 ;

    return(DP_ERRORCLASS(dp)) ;
}

int DP_DownLoadFilms(dp, FilmBuffer, nBytes)
    DP_DATA *dp;
	char    *FilmBuffer;
	int     nBytes;
	
	{
	return (sc_film_download(FilmBuffer, nBytes));
	}
