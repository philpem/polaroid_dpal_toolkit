;  DEC/CMS REPLACEMENT HISTORY, Element POLATIME.ASM
;  *5     3-MAY-1991 12:08:25 BROGNA ""
;  *4    18-JAN-1991 04:55:58 BROGNA "MSC 6.0x"
;  *3    29-NOV-1990 16:58:53 BROGNA "Version 2.21"
;  *2     3-OCT-1990 15:14:53 BROGNA "Single data structure"
;  *1    12-SEP-1990 11:03:29 BROGNA "ToolKit Upgrade"
;  DEC/CMS REPLACEMENT HISTORY, Element POLATIME.ASM
; PolaTime.ASM - clock routines
;
; Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation
;

include moddef.inc

pubproc WaitForTicks

data_seg

ClockLow       dw 0
ClockHigh      dw 0

code_seg

NumTicks     equ     @ab

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;WaitForTicks(NumTicks)
;   int   NumTicks   ; number of clock ticks to wait
;    
procedure WaitForTicks

   push     bp
   mov      bp, sp
   push     cx
   push     dx

   xor      ax, ax
   int      1Ah

   mov      ClockHigh, cx
   mov      ClockLow, dx
   mov      ax, NumTicks[bp]   ; number of ticks to wait for
   add      ClockLow, ax
   adc      ClockHigh, 0
   cmp      ClockHigh, 24
   jb       WFT1
   mov      ClockHigh, 0

WFT0:
   xor      ax, ax             ; wait for a new day
   int      1Ah
   cmp      cx, 0
   jne      WFT0

WFT1:
   xor      ax, ax
   int      1Ah
   cmp      cx, ClockHigh
   jb       WFT1
   ja       WFT3

WFT2:
   cmp      dx, ClockLow
   jb       WFT1

WFT3:
   pop      dx
   pop      cx
   pop      bp

   ret

endproc WaitForTicks

END
