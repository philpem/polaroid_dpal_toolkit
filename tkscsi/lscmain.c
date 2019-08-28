/*  DEC/CMS REPLACEMENT HISTORY, Element LSCMAIN.C */
/*  *2     3-MAY-1991 12:07:32 BROGNA "" */
/*  *1     5-NOV-1990 17:46:59 GRABEL "INITIAL" */
/*  DEC/CMS REPLACEMENT HISTORY, Element LSCMAIN.C */
/* lscmain.c
 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
	 
    main front end for the Digital Palette Toolkit Demos for 
    LightSpeed C.  Adapted from the unix main.c sources from LSC

Functions:

main:
    The demos expect a stdin and stdout for their terminal io.  LightSpeed
    C provides a nice mechanism to do this.  This front end provides a
    mechanism to get command line arguments into the demo main functions.
    In demosup.h, #define's are provided to turn all of the demo main's
    into _main's. This entry function will gobble up command line args
    and pass them to the demos.

mac_exit:
    All of the demo's will exit to mac_exit in this source.  We'll wait
    for a key stroke from the user so the window isn't shut too quickly.
        
difftime:
    This is an ANSI library function which many of the demos use to
    compute elapsed time.  It's not available in the LSC libraries so
    it's provided here.
        
        Some parts:
        (C) Copyright 1985, THINK Technologies, Inc.  All rights reserved.
*/

#include <stdio.h>
#include <ctype.h>

#include "dpalette.h"

#define DEMO_MAIN_MAIN
#include "demosup.h"

#define MAX_ARGS    50

#ifndef true
#define true                          1
#define false                         0
#endif

static  int         argc            = 1;        /* final argument count  */
static  char        *argv[MAX_ARGS] = { "" };   /* array of pointers     */
static  char        command[256];               /* input line buffer     */

/* New main routine.  Prompts for command line then calls user's main   */
void main()
{
    char            c;                  /* temp for EOLN check   */
    register char   *cp;                /* index in command line */
    int i;
    
    Click_On( 0 ) ;     /* turn the 'click box to exit' window off */
    
    printf( "Enter command> " );
    gets( &command );                   /* allow user to edit    */
    cp = &command[0];                   /* start of buffer       */
    argv[0] = "";                       /* program name is NULL  */
    while (argc < MAX_ARGS)
    { /* up to MAX_ARGS entries */
        while (isspace( *cp++ ));
        if ( !*--cp )
            break;
        else
        { /* either an argument or a filename */
            argv[argc++] = cp;
            while ( *++cp && !isspace( *cp ) );
            c = *cp;
            *cp++ = '\0';
            if (!c)
                break;
        }
    }
    _main( argc, argv );
    
    mac_exit() ;
}


mac_exit()
{
    printf("\n\n** program complete, hit any key to exit **") ;
    getchar() ;
    exit() ;
}


double difftime( currtime, oldtime )
time_t currtime, oldtime ;
{
    return( currtime - oldtime ) ;
}

