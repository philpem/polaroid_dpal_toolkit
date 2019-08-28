/*  DEC/CMS REPLACEMENT HISTORY, Element SLEEP.C */
/*  *4     3-MAY-1991 12:14:21 BROGNA "" */
/*  *3    18-JAN-1991 04:57:58 BROGNA "MSC 6.0x" */
/*  *2    16-JAN-1991 09:06:21 CAMPBELL_T "180 Cape work, faster in box, compile under MSC 5" */
/*  *1     1-OCT-1990 21:30:48 CAMPBELL_T "Support code for toolkit evalutation programs" */
/*  DEC/CMS REPLACEMENT HISTORY, Element SLEEP.C */
/* sleep.c
 Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
	 
This has a sleep() function for those run-time libraries that don't provide it

28-sep-90  tc   Initial

Table of contents
    sleep               Suspends execution for "n" seconds
*/

#include <stdio.h>
#include <time.h>

/* sleep

Delays execution for "n" seconds.

This version continually asks the system for the time and returns
when the desired seconds have expired.

21-sep-90  tc   Initial

*/

void
sleep( int n )
{
    /* Get the time at the start */
    time_t start_time = time(NULL) ;

    while ( difftime( time(NULL), start_time) < (float) n ) ;

    return ;
}


