**
* Learning Amiga hardware-direct development in regeards of bobs,
* sprites and all the gfx stuff.
*
*
* Development environment
*   (1) DEVELOPING, TESTING, DEBUGGING, BUILD
*       Amiga with OS 3.1.4 on WinUAE
*       NDK for AmigaOS 3.1
*       O.M.A. assembler 3.0 and its debugger DBug
*       CubicIDE for coding
*
* Build
*   1a) Asm
*    OMA file.asm
*
*   1b) Asm (debug)
*    OMA file.asm -c qs
*
*   2) Link
*    DLink FROM file.o TO file LIB ACOM:Libraries/amiga.lib
*
*
* Author: Uwe Rosner
*    https://github.com/rosneru
**

_custom		equ	$dff000
_ciaa		equ	$bfe001

        include "dos/dos.i"
        include "dos/dos_lib.i"
        include "exec/exec_lib.i"
        include "exec/memory.i"
        include "graphics/graphics_lib.i"
        include "graphics/GfxBase.i"
        include "hardware/custom.i"
        include "hardware/cia.i"


*======================================================================
* Initializations
*======================================================================
        ;Entry point
_main:

        ;
        ; Open dos.library
        ;
        lea     dosname(pc),a1
        moveq   #36,d0              ;36 needed for VPrintf; gfx.lib will
                                    ;then make the proper v39 check

        CALLEXEC OpenLibrary
        beq     Exit
        move.l  d0,_DOSBase

        ;
        ; Open graphics.library
        ;
        lea     gfxname(pc),a1
        moveq.l #36,d0
        CALLEXEC OpenLibrary
        move.l  #strErrOpenGfx,d1   ;The error message if it failed
        move.l  d0,_GfxBase
        beq     Exit_err

        ;
        ; Allocate memory for picture
        ;
        move.l  #81920,d0           ;Size of needed memory
        move.l  #MEMF_CHIP,d1       ;It must be Chip memory
        or.l    #MEMF_CLEAR,d1      ;New memory should be cleared
        CALLEXEC AllocVec
        move.l  #strErrAllocBgImg,d1 ;The error message if it failed
        move.l  d0,picture          ;Save ptr of allocated memory
        beq     Exit_err            ;No memory has been reserved

        ; Load the Background image at reserved memory address
        move.l  #bgImgName,d1       ;Function expects file name in d1..
        move.l  d0,d2               ;..and buf addr in d2
        move.l  #81920,d3           ;..and buf len in d3
        bsr     Load_file_to_buf
        move.l  #strErrLoadBgImg,d1 ;The error message if it failed
        tst.l   d0                  ;If d0 is zero
        bne     Exit_err            ;the file couldn't be loaded
        ;
        ; Switch off current copper list without smashing the current
        ; screen. The order of LoadView(0) and 2xWaitTOF() is set by
        ; Commodore
        ;
        movea.l _GfxBase,a6             ;Use graphics.library
        move.l  gb_ActiView(a6),oldview ;Save current view

clearview
        sub.l   a1,a1                   ;Clear a1 to display no view
        jsr     _LVOLoadView(a6)
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)

        cmp.l   #0,gb_ActiView(a6)  ; Any other view appeared?
        bne.s   clearview           ; If so wipe it.

        jsr     _LVOOwnBlitter(a6)

        CALLEXEC Forbid

*======================================================================
* Main
*======================================================================
        ;
        ; Load address of allocated memory for the background picture
        ; and set the bitplane pointers for all (here: three) planes
        ; in the copper list
        ;
        move.l  picture,d0
        move.l  #20480,d1           ;The size of one bitplane in bytes
                                    ;640/8*256 = 20480


        move.w  d0,plane1+6
        swap    d0
        move.w  d0,plane1+2

        swap    d0
        add.l   d1,d0

        move.w  d0,plane2+6
        swap    d0
        move.w  d0,plane2+2

        swap    d0
        add.l   d1,d0

        move.w  d0,plane3+6
        swap    d0
        move.w  d0,plane3+2

        swap    d0
        add.l   d1,d0

        move.w  d0,plane4+6
        swap    d0
        move.w  d0,plane4+2

        ; Activate the copperlist
        lea     _custom,a1
        move.l  #copperlist,cop1lc(a1)

        ; Activate the bitplane DMA
        move.w  #$8100,dmacon(a1)
loop:
        move.l  $dff004,d0              ;Wait for the beam (WaitTOF?)
        and.l   #$fff00,d0
        cmp.l   #$00003000,d0
        bne.s   loop

        btst    #CIAB_GAMEPORT0,_ciaa   ;Mouse button pressed
        bne     loop


*======================================================================
* Clean up
*======================================================================

        CALLEXEC Permit

        movea.l _GfxBase,a6
        jsr     _LVODisownBlitter(a6)


Exit:
        move.l  _GfxBase,a6         ;Use graphics.library
        move.l  oldview,d0          ;Restore former view
        beq     .ex1                ;But only if there was one
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)
        move.l  oldview,a1
        jsr     _LVOLoadView(a6)

.ex1:
        ; Start former copper list
        lea.l   _custom,a1
        move.l  gb_copinit(a6),cop1lc(a1)

        ; Free memory if it was allocated successfully
        move.l  picture,d0
        tst.l   d0
        beq     .ex2
        move.l  d0,a1
        CALLEXEC FreeVec

.ex2:
        ; Close graphics.library
        move.l  _GfxBase,d0         ;Verify: LibBase needed in d-reg
        beq     .ex3
        move.l  d0,a1               ;Closing: LibBase needed in a1
        CALLEXEC CloseLibrary

.ex3:
        ; Close dos.library
        move.l  _DOSBase,d0         ;Verify: LibBase needed in d-reg
        beq     .ex4
        move.l  d0,a1               ;Closing: LibBase needed in a1
        CALLEXEC CloseLibrary


.ex4:
;        move.l  #$8020,$dff096      ;Enable sprites
        lea     _custom,a1
        move.w  #$8020,dmacon(a1)
        rts


; NOTE: a1 must be loaded with the address of an error text before!
Exit_err:
        movea.l _DOSBase,a6
        move.l  0,d2
        jsr     _LVOVPrintf(a6)
        bra     Exit


*======================================================================
* Sub
*======================================================================

; Function Load_file_to_buf
;   Loads a file into a memory buffer which must have been reserved
;   before. Opens the file before and closes it after loading.
;
; Parameters
;   d1: file name
;   d2: buf address
;   d3: buf size
;
; Returns
;   d0: zero on success or error code
Load_file_to_buf:
        movem.l d2-d6/a2-a6,-(sp)
        move.l  d2,d4                       ;Save buf addr for Read

        move.l  _DOSBase,a6

        move.l  #MODE_OLDFILE,d2
        jsr     _LVOOpen(a6)
        tst.l   d0
        beq     Load_file_to_buf_err_open   ;d0 = 0, File not opened

        move.l  d0,d5                       ;Save file handle for Close

        move.l  d0,d1                       ;But Read needs it in d1
        move.l  d4,d2                       ;Restore buf addr from d4
        jsr     _LVORead(a6)
        tst.l   d0
        blt     Load_file_to_buf_err_read   ;d0 < 0; Read error

        move.l  d5,d1                       ;Restore file handle from d5
        jsr     _LVOClose(a6)

        movem.l (sp)+,d2-d6/a2-a6
        sub.l   d0,d0                       ;Success: d0 = 0
        rts

Load_file_to_buf_err_read
        move.l  d5,d1                       ;Restore file handle from d5
        jsr     _LVOClose(a6)

        movem.l (sp)+,d2-d6/a2-a6
        move.l  #-1,d0                      ;Error: d0 = error code
        rts


Load_file_to_buf_err_open
        movem.l (sp)+,d2-d6/a2-a6
        move.l  #-2,d0                      ;Error: d0 = error code
        rts


*======================================================================
* Data
*======================================================================

dosname             DOSNAME             ;dos.i
gfxname             GRAPHICSNAME        ;gfxbase.i
                    even

_DOSBase            ds.l    1
_GfxBase            ds.l    1

oldview             ds.l    1

picture             even
                    ds.l    1

tmpvar              even
                    ds.l    1

strErrOpenGfx       even
                    dc.b          'Can''t open graphics.library v39.',10,0

strErrAllocBgImg    even
                    dc.b          'Can''t allocate memory for background image.',10,0

strErrLoadBgImg     even
                    dc.b          'Can''t load background image.',10,0


bgImgName           even
                    dc.b          'gfx/background.raw',0

                    SECTION "dma",data,chip
                    even
copperlist
;                    dc.w    $0207,$fffe ;Required for AGA machines
                    dc.w    dmacon,$0020 ;Disable sprites
                    dc.w    diwstrt,$2C81
                    dc.w    diwstop,$2CC1
                    dc.w    ddfstrt,$003C
                    dc.w    ddfstop,$00d4
                    dc.w    bplcon0,$C200   ; Hires, 4 bitplanes
                    dc.w    bplcon1,$0000
                    dc.w    bplcon2,$0000
                    dc.w    bpl1mod,$0000
                    dc.w    bpl2mod,$0000
plane1              dc.w    bplpt,$0000
                    dc.w    bplpt+2,$0000
plane2              dc.w    bplpt+4,$0000
                    dc.w    bplpt+6,$0000
plane3              dc.w    bplpt+8,$0000
                    dc.w    bplpt+10,$0000
plane4              dc.w    bplpt+12,$0000
                    dc.w    bplpt+14,$0000
                    dc.w    color+0,$0000   ;COLOR00
                    dc.w    color+2,$0222   ;COLOR01
                    dc.w    color+4,$0544   ;COLOR02
                    dc.w    color+6,$0766   ;COLOR03
                    dc.w    color+8,$0ffc   ;COLOR04
                    dc.w    color+10,$0488  ;COLOR05
                    dc.w    color+12,$08a9  ;COLOR06
                    dc.w    color+14,$0696  ;COLOR07
                    dc.w    color+16,$0ac7  ;COLOR08
                    dc.w    color+18,$0991  ;COLOR09
                    dc.w    color+20,$0bb2  ;COLOR10
                    dc.w    color+22,$0d92  ;COLOR11
                    dc.w    color+24,$0fb2  ;COLOR12
                    dc.w    color+26,$0d50  ;COLOR13
                    dc.w    color+28,$0c21  ;COLOR14
                    dc.w    color+30,$0f43  ;COLOR15
;                    dc.w    $7007,$fffe     ;WAIT
;                    dc.w    color,$0fff     ;COLOR00 -> white
;                    dc.w    $e007,$fffe     ;WAIT
;                    dc.w    color,$0f00     ;COLOR00 -> red
;                    dc.w    $ff07,$fffe  ;Wait for last ntsc line
;                    dc.w    dmacon,$8020    ;Enable sprites
                    dc.w    $ffff,$fffe     ;Waiting for impossible position

                END

