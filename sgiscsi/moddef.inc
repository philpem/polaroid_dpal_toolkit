;  DEC/CMS REPLACEMENT HISTORY, Element MODDEF.INC
;  *2     3-MAY-1991 12:07:48 BROGNA ""
;  *1    29-NOV-1990 17:02:28 BROGNA "Version 2.21"
;  DEC/CMS REPLACEMENT HISTORY, Element MODDEF.INC
; define memory model for library assembly code
; Copyright (C) 1988, 1989, 1990, 1991 Polaroid Corporation
;
; The memory model (LARGE, MEDIUM, COMPACT, or SMALL) must be DEFINEd before
; this file is INCLUDEd.  Either define it as a command line arg to masm or
; define it in the source code file before the include command.

DOSSEG

ifdef LARGE
   .MODEL large
   if1
      %out LARGE Model Assembly
   endif
endif

ifdef MEDIUM
   .MODEL medium
   if1
      %out MEDIUM Model Assembly
   endif
endif

ifdef COMPACT
   .MODEL compact
   if1
      %out COMPACT Model Assembly
   endif
endif

ifdef SMALL
   .MODEL small
   if1
      %out SMALL Model Assembly
   endif
endif

if @codesize
   .CODE @filename
else
   .CODE
endif

if @datasize
   .FARDATA
   .FARDATA?
endif

   .DATA
   .CONST
   .DATA?
   .STACK

if @codesize
   @ab equ 6
else
   @ab equ 4
endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

code_seg   macro
   if @codesize
      .CODE @filename
   else
      .CODE
   endif
endm

data_seg   macro
   .DATA
endm

extfunct macro X
   EXTRN _&X:PROC
endm

pubproc macro X
   PUBLIC _&X
endm

procedure macro X
   _&X PROC
endm

endproc macro X
   _&X ENDP
endm

callx macro X
   if @codesize
      call far ptr _&X
   else
      call _&X
   endif
endm
