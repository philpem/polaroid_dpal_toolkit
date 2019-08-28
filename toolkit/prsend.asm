;  DEC/CMS REPLACEMENT HISTORY, Element PRSEND.ASM
;  *9     3-MAY-1991 12:08:35 BROGNA ""
;  *8    18-JAN-1991 04:56:21 BROGNA "MSC 6.0x / Midnight bug fix"
;  *7    29-NOV-1990 16:59:00 BROGNA "Version 2.21"
;  *6    11-OCT-1990 16:34:15 BROGNA "Real Port Checking"
;  *5     3-OCT-1990 15:15:02 BROGNA "Single data structure"
;  *4    12-SEP-1990 11:03:37 BROGNA "ToolKit Upgrade"
;  *3     1-MAY-1990 09:06:37 BROGNA "RT Clock Fix"
;  *2     6-APR-1990 14:10:32 BROGNA "release 144"
;  *1    15-MAR-1990 11:35:12 BROGNA "new toolkit stuff"
;  DEC/CMS REPLACEMENT HISTORY, Element PRSEND.ASM
; PrSend.ASM - fast communication over a parallel port that by-passes
;             the BIOS
;
; Copyright (C) 1988, 1989, 1990, 1991  Polaroid Corporation
;
; This module contains four routines for accessing a parallel port on
; an IBM PC, IBM PC/AT, or compatible.  
;
; The first routine, PrInit, must be called before any of the others.  It 
; takes a single input which is the port number (1, 2, or 3) and accesses 
; low memory to obtain the port base address for the indicated parallel 
; port.  The address is saved in the variable pportbase for later use by 
; the other routines.
; 
; PrSend is used to write a buffer of data to the parallel port.  It takes 
; as inputs the buffer pointer, the number of bytes to be sent, a flag that
; indicates whether the routine should check for errors on the parallel port.
; and an argument which indicates how long the routine should try to send data
; thru the port when the busy line is set.  The only time errors should not be
; checked for is when sending the Clear Error command to the Digital Palette
; to clear an error that has already occurred.  This routine returns 0 if no
; error or a negative error number code if an error occurs.
; 
; The routine PrGet returns the next byte in a message being sent from 
; the Digital Palette to the computer.  It takes one argument, how long to
; keep trying to communicate with the Digital Palette when the busy line is
; set.
;
; PrError returns whether an error has occurred, and if so then the type of
; error.
;
; Note that for purposes of speed, registers are used in non-standard
; ways.  E.g. the di register is used to store the strobe values.  Such
; registers are restored when these routines exit.
;
include moddef.inc

pubproc PrInit
pubproc PrSend
pubproc PrGet
pubproc PrErrors

data_seg

PportBase      dw 0                 ; parallel port base address
PgetBits       db 7,6,5,4,3,2,1,0   ; bit numbers for getnextbyte routine

code_seg

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This data is located within the code segment for speed.
;
ClockLow       dw 0
ClockHigh      dw 0
TimeOutTicks   dw 1092              ; 60 seconds
ErrorByteCount dw 5                 ; only want to check 5 byte data packet

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; port status values:
;
;   parallel port registers: (# is replaced by 3 (port 1), 2 (port 2))
;
;      0x#78 - printer port data latch
;
;      0x#79 - printer status
;         bit 7 -BUSY
;         bit 6 -ACK
;         bit 5 +PE
;         bit 4 +SLCT
;         bit 3 -Error
;
;      0x#7a - printer control
;         bit 4 +IRQ Enable
;         bit 3 +SLCT IN
;         bit 2 -INIT
;         bit 1 +AUTO FD XT
;         bit 0 +STROBE
;
STATUSMASK    equ   0880H ; high byte error mask, low byte busy mask
NOERRCHKMSK   equ   0080H ; mask to ignore error checking
STROBEMASK    equ   0405H ; high byte strobe low ; low byte strobe high
ACKNOWLEDGE   equ   40H   ; acknowledge line mask + Select
PAPEREND      equ   20H   ; paper end line mask
SELECT        equ   10H   ; select line mask
PORTMASK1     equ  0AAH   ; 
PORTMASK2     equ   55H   ;

PACKETLENGTH  equ    5    ; 5 byte packet

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; error conditions
;
LOGICERR      equ   -1    ; command logical/synchronous error
HARDWERR      equ   -2    ; hardware/asynchronous error
TIMEOUT       equ   -3    ; timeout condition
DISCONNECTED  equ   -4    ; select line not set
UNKNOWNPORT   equ   -5    ; port unknown to system

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SendStrobes macro

; macro SendStrobes sends a high strobe then a low strobe to the printer
;   control port.
;
;      This macro expects:
;         es   =>   low byte = high strobe, high byte = low strobe
;         dx   =>   printer control port
;      This macro changes the following registers:
;         ax
;
   mov      ax, di        ; ah => low strobe, al => high strobe
   out      dx, al        ; send high strobe first
   xchg     al, ah        ; swap strobes
   jmp      $+2           ; insure strobe pulse long enough
                          ; has been checked on a 20MHz Compaq
   out      dx, al        ; strobe STB low

; end of macro SendStrobes
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ErrorCheck macro
   local   ErrorTest, CheckByteCount, ErrorCheckEnd

; macro ErrorCheck
;   checks the paper end and the error lines of the printer status port
;   to determine if any errors have been reported by the filmprinter
;
;   This macro expects:
;      al => contents of status port
;      bh => error mask (either busy mask or 0ffh to ignore error test)
;   This macro changes the following registers:
;      ax gets 0 or negative error number
;
   test     al, bh        ;
   jz       ErrorTest     ; if error line set figure out what kind
   xor      ax, ax        ; set no error
   jmp      CheckByteCount

ErrorTest:
   test     al, PAPEREND  ; is bit 5 high?
   mov      ax, LOGICERR  ; No, we have a logic error
   jz       CheckByteCount ; Yes, both lines have been set
   mov      ax, HARDWERR  ; and we have a hardware error

CheckByteCount:
   dec      cs:ErrorByteCount
   jnz      ErrorCheckEnd
   mov      bx, NOERRCHKMSK

ErrorCheckEnd:
; end of macro ErrorCheck
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BusyCheck macro
 local   MT0, MT1, T0, T1, T2, BL0, BL1, BL2, BL3, TimedOut, NotBusy, BusyEnd

; macro BusyCheck checks the printer status port to verify correct
;   communications.
;
;   This macro expects:
;      dx   =>   printer status port
;      bl   =>   busy mask               ; bh => error mask
;   This macro changes the following registers:
;      di, ax
;
   push     di

   in       al, dx
   test     al, ACKNOWLEDGE
   jz       MT0
   test     al, bl
   jz       MT0
   jmp      NotBusy

MT0:
   xor      di, di

MT1:
   in       al, dx
   test     al, ACKNOWLEDGE
   jz       T0
   test     al, bl
   jz       T0
   jmp      NotBusy
   dec      di
   jnz      MT1

T0:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This section reads clock ticks from low memory    ;
;                                                   ;
   push     bx                                      ;
   mov      ax, 040H                                ;
   mov      es, ax                                  ;
   mov      bx, 06cH                                ;
   cli                         ; disable interrupts ;
   mov      ax, es:[bx]                             ;
   mov      cs:ClockLow, ax                         ;
   inc      bx                                      ;
   inc      bx                                      ;
   mov      ax, es:[bx]                             ;
   sti                         ; enable interrupts  ;
   mov      cs:ClockHigh, ax                        ;
   mov      ax, cs:TimeOutTicks                     ;
   add      cs:ClockLow, ax                         ;
   adc      cs:ClockHigh, 0                         ;
   pop      bx                                      ;
   cmp      cs:ClockHigh, 24                        ;
   jb       BL0                                     ;
   mov      cs:ClockHigh, 0                         ;
T1:                                                 ;
   in       al, dx                                  ;
   test     al, ACKNOWLEDGE                         ;
   jz       BL1                                     ;
   test     al, bl                                  ;
   jz       T2                                      ;
   jmp      NotBusy                                 ;
T2:                                                 ;
   push     bx                 ; wait for next day  ;
   mov      ax, 040H                                ;
   mov      es, ax                                  ;
   mov      bx, 06eH                                ;
   mov      ax, es:[bx]                             ;
   pop      bx                                      ;   
   cmp      ax, 0                                   ;
   jne      T2                                      ;
;                                                   ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BL0:
   in       al, dx
   test     al, ACKNOWLEDGE
   jz       BL1
   test     al, bl
   jnz      NotBusy

BL1:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This section reads clock ticks from low memory ;
;                                                ;
   push     bx                                   ;
   mov      ax, 040H                             ;
   mov      es, ax                               ;
   mov      bx, 06eH                             ;
;                                                ;
   mov      ax, es:[bx]                          ;
   cmp      ax, cs:ClockHigh                     ;
   ja       BL3                                  ;
   je       BL2                                  ;
   pop      bx                                   ;
   jmp      BL0                                  ;
BL2:                                             ;
   dec      bx                                   ;
   dec      bx                                   ;
   mov      ax, es:[bx]                          ;
;                                                ;
   cmp      ax, cs:ClockLow                      ;
BL3:                                             ;
   pop      bx                                   ;
;                                                ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   ja       TimedOut
   jmp      BL0

TimedOut:
   sti                     ; make sure interrupts are enabled
   mov      ax, TIMEOUT
   jmp      BusyEnd

NotBusy:
   xor      ax, ax
   or       bh, bh
   jz       BusyEnd
   in       al, dx
   ErrorCheck              ; macro to check for errors

BusyEnd:

   pop      di
; end of macro BusyCheck
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SetByteBit macro
   local   SetByteBitEnd

; macro SetByteBits checks the paper end line to determine whether the
;   current bit in the message byte should be 1 or 0.  
;
;   This macro expects:
;      ah   => previous bits read
;      dx   => printer status port
;
;   This macro changes the following registers:
;      ah   => previous bits shifted left 1 bit and current bit in bit 0
;      al   => destroyed
;
   shl      ah, 1          ; shift previous bits
   in       al, dx         ; bit 5 used to ah bit set
   test     al, PAPEREND   ; check for paper end
   jz       SetByteBitEnd  ; 0 => current bit is 0, and done
   or       ah, 1          ; 1 => set current bit in ah

SetByteBitEnd:

; end of macro SetByteBit
endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

portnum     equ     @ab

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;PrInit(PortNum)
;   int   PortNum   ; PortNum = 1,2, or 3
;    
; Sets parallel port base address for later use by other routines.
;   This routine has to be called before any other routines in this module.
;
; Returns 0 if no error or 1 if parallel port base address is 0
;
procedure PrInit

   push     bp
   mov      bp, sp
   push     es

   mov      ax, UNKNOWNPORT   ; ax <= error return
   mov      bx, PortNum[bp]   ; bx <= port number
   cmp      bx, 0             ; test bx = 1,2, or 3
   jle      PrInitEnd
   cmp      bx, 3
   jg       PrInitEnd

   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ; get the parallel port base address from low memory.  The base address of
   ; port 1 is at offset 0x40:8, port 2 at 0x40:10, port 3 at 0x40:12
   ;
   dec      bx                ; offset of pointer to port io address
   shl      bx, 1            
   add      bx, 8

   mov      ax, 40h           ; segment of pointer to port io address
   mov      es, ax

   mov      ax, es:[bx]       ; ax <= port io address
   mov      PportBase, ax     ; save for later use
   or       ax, ax            ; should be non zero
   jnz      RealCheck1        ; if it is check out the port, device
   mov      ax, UNKNOWNPORT   ; otherwise return error
   jmp      PrInitEnd

RealCheck1:
   mov      dx, PportBase     ; data latch for parallel port
   mov      al, PORTMASK1     ; value check
   out      dx, al            ; send it out
   in       al, dx            ; bring it back in
   xor      al, PORTMASK1     ; should zero out
   jz       RealCheck2        ; zero, do another test
   mov      ax, DISCONNECTED  ; return error
   jmp      PrInitEnd

RealCheck2:
   mov      al, PORTMASK2     ; value check
   out      dx, al            ; send it out
   in       al, dx            ; bring it back in
   xor      al, PORTMASK2     ; should zero out
   jz       PortOk            ; zero, check for device
   mov      ax, DISCONNECTED  ; return error
   jmp      PrInitEnd

PortOk:
   inc      dx
   in       al, dx
   dec      dx
   test     al, SELECT
   jnz      DeviceFound
   mov      ax, DISCONNECTED  ; ax <= error return
   jmp      PrInitEnd

DeviceFound:
   xor      ax, ax            ; ax = 0 <= no error

PrInitEnd:
   pop      es
   pop      bp

   ret

endproc PrInit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PrSend(buf, nbytes, errorflg, direction, maxtimeoutticks)
;   char *buf             /* pointer to buffer of characters */
;   int   nbytes          /* number of bytes to send */
;   int   errorflg        /* 1 => check for errors ; 0 => don't check
;   int   direction       /* how to set the direction flag */
;   int   maxtimeoutticks /* maximum clock ticks to for retries if the busy
;                          * line is set. (18.2 ticks / second)
;
; Sends nbytes of data from buf to the Digital Palette.
;
;   returns   0   if no error
;            -1   if logical command error reported by Digital Palette
;            -2   if hardware error reported by Digital Palette
;            -3   if Digital Palette timeout condition
;  
Buf         equ   @ab
if @datasize
nBytes      equ @ab+4
ErrorFlg    equ @ab+6
Backward    equ @ab+8
TimeOutMax  equ @ab+10
else
nBytes      equ @ab+2
ErrorFlg    equ @ab+4
Backward    equ @ab+6
TimeOutMax  equ @ab+8
endif

procedure PrSend
   push     bp
   mov      bp, sp
   push     bx
   push     cx
   push     dx
   push     di                ; save important registers
   push     si
   push     es
   push     ds

   mov      dx, PportBase        ; data latch for parallel port

   inc      dx
   in       al, dx
   dec      dx
   test     al, SELECT
   jnz      sSetup
   jmp      NotSelect

sSetup:
   mov      ax, TimeOutMax[bp]   ; set the maximum time out duration
   mov      cs:TimeOutTicks, ax
   mov      cs:ErrorByteCount, PACKETLENGTH ; number of chars to errorcheck

if @datasize
   lds      si, Buf[bp]       ; get the beginning of the Buffer in ds:si
else
   mov      si, Buf[bp]       ; get the beginning of the Buffer in si
endif
   mov      cx, nBytes[bp]    ; get the number of bytes to write in cx
   or       cx, cx            ; test for zero chars
   jnz      sGo               ; set up to go
   jmp      LastChk

sGo:
   mov      bx, STATUSMASK    ; bh <= error mask , bl <= busy mask
   mov      ax, STROBEMASK    ; ah <= strobe low , al <= strobe high
   mov      di, ax            ; di <= strobes

   mov      ax, ErrorFlg[bp]  ; ax == 0 => do not check for errors
   or       ax, ax
   jnz      rGo               ; ready to go
   mov      bx, NOERRCHKMSK   ; bh <= mask that ignores error
rGo:
   mov      ax, Backward[bp]  ; check for reverse buffer
   or       ax, ax
   jz       OutChar           ; not reverse
   std                        ; set the direction flag, it is always
                              ; cleared before returning.
OutChar:
   lodsb                      ; get the character to send

   out      dx, al            ; dx already has the portbase
   inc      dx                ; point to the status port

   BusyCheck                  ; macro to verify printer communication

   or       ax, ax            ; any errors?
   jz       sStrobes          ; zero => ok
   jmp      PrSendEnd

sStrobes:
   inc      dx                ; dx => printer control port
   SendStrobes                ; macro to send high and low strobes
   dec      dx
   dec      dx                ; dx => portbase
   dec      cx
   jz       LastChk
   jmp      OutChar

LastChk:
   inc      dx
   in       al, dx
   test     al, SELECT
   jz       NotSelect

   xor      ax, ax
   jmp      PrSendEnd

NotSelect:
   mov      ax, DISCONNECTED
PrSendEnd:

   cld                        ; make sure direction flag is cleared

   pop      ds
   pop      es
   pop      si
   pop      di
   pop      dx
   pop      cx
   pop      bx
   pop      bp

   ret

endproc PrSend


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PrGet()
;
; gets a byte from the film printer by sending bit numbers one at a time.
;
;   entry:   none
;
;   exit:      returns character byte or error code
;               -1 = logic error
;               -2 = hardware error
;               -3 = timeout error
;

gTimeOutMax equ @ab

procedure PrGet
   push     bp
   mov      bp, sp
   push     cx
   push     dx
   push     di                         ; save important registers
   push     si
   push     es

   mov      ax, gTimeOutMax[bp]      ; set the maximum time out duration
   mov      cs:TimeOutTicks, ax
   mov      cs:ErrorByteCount, PACKETLENGTH ; number of chars to errorcheck

   mov      si, offset DGROUP:PgetBits ; si <= bit number pointer

   mov      cx, 8                      ; get the number of bits in cx

   mov      bx, STATUSMASK             ; bh <= error mask , bl <= busy mask
   mov      ax, STROBEMASK             ; ah <= strobe low, al <= strobe high
   mov      di, ax                     ; di <= strobe mask
   xor      ah, ah                     ; result byte ah initialized to 0

gOutChar:
   lodsb                               ; get the character to send
   mov      dx, PportBase              ; data latch for parallel port
   out      dx, al
   inc      dx                         ; point to the status port

   push     ax                         ; save ah
   BusyCheck                           ; macro to verify communications
   or       ax, ax                     ; any errors?
   jz       zeroBit                    ; no, check the message bit
   jmp      gPrError                   ; yes, reset stack and return ax

zerobit:
   pop      ax                         ; restore ah for update
   SetByteBit                          ; macro to get one bit of message byte
   inc      dx                         ; dx => printer control port
   push     ax
   SendStrobes                         ; macro to send high and low strobes
   pop      ax

   dec      cx
   or       cx, cx
   jz       gLastChk
   jmp      gOutChar

   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
   ; make sure no error after last character sent
   ;
gLastChk:
   mov      dx, PportBase              ; dx = data latch for parallel port
   inc      dx                         ; point to the status port

   push     ax                         ; save ah
   BusyCheck                           ; macro to verify communications
   or       ax, ax                     ; any errors?
   jnz      gPrError                   ; non zero => error
                                       ; reset stack and return ax
   pop      ax                         ; restore ah for update
   SetByteBit                          ; macro to get one bit of message byte

gPrSendEnd:
   mov      al, ah                     ; return character in al
   xor      ah, ah
   jmp      PrGetEnd

gPrError:                              ; reset stack pointer
                                       ; ax pushed onto stack before BusyCheck
   add      sp, 2                      ; now ax => error return

PrGetEnd:
   pop      es
   pop      si
   pop      di
   pop      dx
   pop      cx
   pop      bp

   ret

endproc PrGet

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PrErrors()
;
;   returns:
;             0   if no error on printer line
;            -1   if logic error has occured
;            -2   if hardware error has occured
;
procedure PrErrors
   push     bx
   push     dx

   mov      dx, PportBase  ; previously saved parallel port base addr
   inc      dx                     ; point to the control port
   in       al, dx         ; get the control port contents
   mov      bx, STATUSMASK ; bh => error mask , bl <= busy mask
   ErrorCheck              ; macro to check printer control port lines
                           ; ax => error level
   pop      dx
   pop      bx

   ret

endproc PrErrors

END
