/*  DEC/CMS REPLACEMENT HISTORY, Element PATHPARS.C */
/*  *5     3-MAY-1991 12:08:10 BROGNA "" */
/*  *4     1-APR-1991 18:03:27 BROGNA "Toolkit enhancments" */
/*  *3    18-JAN-1991 04:55:52 BROGNA "MSC 6.0x" */
/*  *2     3-OCT-1990 15:14:35 BROGNA "Single data structure" */
/*  *1    12-SEP-1990 11:03:16 BROGNA "ToolKit Upgrade" */
/*  DEC/CMS REPLACEMENT HISTORY, Element PATHPARS.C */
/* PATHPARS.C
 * Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation.
 *
 * Parse path\file into components: Drive, Path, FileName, FileSuffix.
 */

void
PathParse(char *, char *, char *, char *, char *) ;

#define PATHPARS_C

void
PathParse(FPath, Drive, Path, Fname, Fext)
   char   *FPath ;
   char   *Drive ;
   char   *Path ;
   char   *Fname ;
   char   *Fext ;
   {
   char   *ptr ;
   char   *ptr1 ;
   char   *ptr2 ;
   char   *ptr3 ;
   
   ptr1 = 0 ;
   ptr2 = 0 ;
   ptr3 = 0 ;
   for (ptr = FPath ; *ptr ; ptr++)
      {
      if (*ptr == ':')
         ptr1 = ptr ;
      if (*ptr == '/')
         ptr2 = ptr ;
      if (*ptr == '.')
         ptr3 = ptr ;
      }

   for (ptr = FPath ; (*ptr) && (ptr <= ptr1) && (ptr1) ; ptr++)
      *(Drive++) = *ptr ;
   *Drive = 0 ;

   for (ptr = FPath ; (*ptr) && (ptr <= ptr2) && (ptr2) ; ptr++)
      if (ptr > ptr1)
         *(Path++) = *ptr ;
   *Path = 0 ;

   for (ptr = FPath ; (*ptr) && (ptr != ptr3) ; ptr++)
      if (ptr > ptr2)
         *(Fname++) = *ptr ;
   *Fname = 0 ;

   for (ptr = ptr3 ; (ptr) && (*ptr) ; ptr++)
      *(Fext++) = *ptr ;
   *Fext = 0 ;
   }
