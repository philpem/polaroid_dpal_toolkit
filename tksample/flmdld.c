/*  DEC/CMS REPLACEMENT HISTORY, Element FLMDLD.C */
/*  *3    20-JUN-1991 16:03:26 BIELEFELD "fix to msc6.0 warnings" */
/*  *2     3-MAY-1991 12:07:10 BROGNA "" */
/*  *1    25-MAR-1991 16:21:05 BROGNA "New toolkit sample" */
/*  DEC/CMS REPLACEMENT HISTORY, Element FLMDLD.C */
/* tkflmdld.c

Usage: flmdld <film.flm> <slotno>

Retrieves and prints all the film information

15-mar-91  jb   Initial
*/

#include <stdio.h>
#include <stdlib.h>
#include "dpalette.h"
#include "dptkcent.h"
#include "demosup.h"

static unsigned char Pdata[4200];
static DP_DATA  our_dp;

int main(int argc, char **argv)
	
    {
    /* Local declarations */
    DP_DATA *dp = &our_dp ;
    FILE *fh;
	int  i, c;
	int  FilmSize;
	
    if (argc != 3)
		{
		printf ("Use: sflmdld <table.flm> <slot>\n");
		return(-1);
		}
	
    printf ("sflmdld, Writes a film table to a pallette.\n");

    /* Init it */
    if ( DP_InitPrinter(dp, SHOW_BUFFER_WARNING, FALSE) )
    {   /* Error initing */
        printf ("Got an error Initing the film printer!\n") ;
        printf ("%s\n", printer_err_msg(dp) ) ;
        printf ("We will still try to write a film table.\n") ;
    }

    /* Get the film table file */
    if ((fh = fopen(argv[1], "rb")) == NULL)
		{
        printf ("Use: tkflmdld <table.flm> <slot>\n");		
		return(-1);
		}

	/* Get the slot number */
	if ((Pdata[0] = (char)atoi(argv[2])) == 0)
		{
		printf ("Use: tkflmdld <table.flm> <slot>\n");
		return(-1);
		}

	if (Pdata[0] > 19)
		{
		printf ("Slot number sellected out of range.\n");
		printf ("Sellect slot numbers between 0 and 19.\n");
		return(-1);
		}
	
	/* Get the film table data */
	FilmSize = 0;
	while ((c = fgetc(fh)) != EOF)
		{
		Pdata[1+FilmSize] = (char) (c & 0xFF);
		FilmSize++;
		}

	printf("Film table file size = %d bytes long.\n",FilmSize);
	
	/* Close the file */
	fclose(fh);
	
	/* Check the pallette for buffer space. */
	if (!DP_ConfirmBuffer(dp, FilmSize + DP_CMD_PACKET_SIZE, TKWAIT))
		{
		if (DP_ERRORCLASS(dp) = DP_DownLoadFilms(dp, Pdata, FilmSize + 1))
			{
			printf ("Got an error attempting to download a table.\n");
			printf ("FilmSize: %d, Slot: %d\n", FilmSize, (int)Pdata[0]);
			return(-1);
			}
		}
	DP_GetPrinterStatus (dp, ALL_INQUIRIES);
	if (DP_ERRORCLASS(dp))
		{
		printf ("Got an error after attempting to download a table.\n");
		printf ("DP_ERRORCLASS = %d\n",DP_ERRORCLASS(dp));
		return(-1);
		}
	else
		{
		printf ("The film table downloaded successfully!!!\n\n");
		printf ("	SLOT#:		A/R:		FILM NAME:\n\n");
		for (i = 0; i < 20; i++)
			{
			if (i == (int)Pdata[0])
				printf ("-->	%2d		%2d:%2d		%s\n",
				  i,DP_X_ASPECT(dp,i),DP_Y_ASPECT(dp,i),DP_FILM_NAME(dp,i));
			else
			    printf ("	%2d		%2d:%2d		%s\n",
			      i,DP_X_ASPECT(dp,i),DP_Y_ASPECT(dp,i),DP_FILM_NAME(dp,i));
			}
		}
}
