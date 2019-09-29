**
* Learning Amiga development in regeards of bobs, sprites and all the
* graphics stuff.
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
* History
*   18.08.2019 - Moving the gray gradient
*   18.08.2019 - Added a copper gray gradient
*   17.08.2019 - Added error message output if init fails
*   17.08.2019 - Dropped lowlevel.library suppurt; added dos.library
*   22.07.2019 - Modified copper list, displaying three colored blocks
*   21.07.2019 - Dynamic allocation of chip memory for the view
*   21.07.2019 - Using lowlevel.library
*   20.07.2019 - Simplest copperlist
*
* Author: Uwe Rosner
*    https://github.com/rosneru
**

        include "dos/dos.i"
        include "exec/memory.i"
        include "graphics/GfxBase.i"
        include "hardware/custom.i"
        include "hardware/cia.i"

        XDEF    _SysBase
        XDEF    _GfxBase
        XDEF    _DOSBase
        XDEF    _main

        ;
        ; Stating the used LVOs
        ; (coming from amiga.lib / small.lib)
        ;
        XREF    _AbsExecBase
        XREF    _LVOOpenLibrary
        XREF    _LVOCloseLibrary
        XREF    _LVOAllocVec
        XREF    _LVOFreeVec
        XREF    _LVOLoadView
        XREF    _LVOWaitTOF
        XREF    _LVOForbid
        XREF    _LVOPermit
        XREF    _LVOOwnBlitter
        XREF    _LVODisownBlitter
        XREF    _LVOOpen
        XREF    _LVOVPrintf
        XREF    _LVORead
        XREF    _custom
        XREF    _ciaa


*======================================================================
* Initializations
*======================================================================
        ;Entry point
_main:

        movea.l _AbsExecBase,a6
        move.l  a6,_SysBase

        ;
        ; Open dos.library
        ;
        lea     dosname(pc),a1
        moveq   #36,d0              ;36 needed for VPrintf; gfx.lib will
                                    ;then make the proper v39 check

        jsr     _LVOOpenLibrary(a6)
        move.l  d0,_DOSBase
        beq     Exit

        ;
        ; Open graphics.library
        ;
        lea     gfxname(pc),a1
        moveq.l #39,d0
        jsr     _LVOOpenLibrary(a6)
        move.l  #strErrOpenGfx,d1   ;The error message if it failed
        move.l  d0,_GfxBase
        beq     Exit_err

        ;
        ; Allocate memory for picture
        ;
        move.l  #61440,d0           ;Size of needed memory
        move.l  #MEMF_CHIP,d1       ;It must be Chip memory
        or.l    #MEMF_CLEAR,d1      ;New memory should be cleared
        jsr     _LVOAllocVec(a6)
        move.l  #strErrAllocImgMem,d1 ;The error message if it failed
        move.l  d0,picture          ;Save ptr of allocated memory
        beq     Exit_err            ;No memory has been reserved

        ; Load the Background image
        bsr Load_raw_image

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

        movea.l _SysBase,a6
        jsr     _LVOForbid(a6)

*======================================================================
* Main
*======================================================================
        ;
        ; Load address of allocated memory for the picture
        ;
        move.l  picture,d0
        move.w  d0,pl1+6
        swap    d0
        move.w  d0,pl1+2

        ; Activate the copperlist
        lea     _custom,a1
        move.l  #copperlist,cop1lc(a1)

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


        movea.l _SysBase,a6
        jsr     _LVOPermit(a6)

        movea.l _GfxBase,a6
        jsr     _LVODisownBlitter(a6)


Exit:
        move.l  oldview,d0          ;Restore former view
        beq     Exit_1              ;But only if there was one
        move.l  _GfxBase,a6         ;Use graphics.library
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)
        move.l  oldview,a1
        jsr     _LVOLoadView(a6)
        lea.l   _custom,a1
        move.l  gb_copinit(a6),cop1lc(a1) ;Start former copper list


Exit_1:
        ; Free memory if it was allocated successfully
        movea.l _SysBase,a6
        move.l  picture,d0
        tst.l   d0
        beq     Exit_2
        move.l  d0,a1
        jsr     _LVOFreeVec(a6)

Exit_2:
        ; Close graphics.library
        movea.l _SysBase,a6
        move.l  _GfxBase,d0         ;Verify: LibBase needed in d-reg
        beq     Exit_4
        move.l  d0,a1               ;Closing: LibBase needed in a1
        jsr     _LVOCloseLibrary(a6)

Exit_3:
        ; Close dos.library
        movea.l _SysBase,a6
        move.l  _DOSBase,d0         ;Verify: LibBase needed in d-reg
        beq     Exit_3
        move.l  d0,a1               ;Closing: LibBase needed in a1
        jsr     _LVOCloseLibrary(a6)


Exit_4:
        move.l  #$8020,$dff096      ;Enable sprites
        rts;


; NOTE: a1 must be loaded with the address of an error text before!
Exit_err:
        movea.l _DOSBase,a6
        move.l  0,d2
        jsr     _LVOVPrintf(a6)
        bra     Exit


*======================================================================
* Sub
*======================================================================

Load_raw_image:
;        lea     cins,a0
        rts


*======================================================================
* Data
*======================================================================

dosname         DOSNAME             ;dos.i
gfxname         GRAPHICSNAME        ;gfxbase.i
                even

_DOSBase        ds.l    1
_GfxBase        ds.l    1
_SysBase        ds.l    1

oldview         ds.l    1

picture         cnop    0,2
                ds.l    1

strErrOpenGfx   dc.b          'Can''t open graphics.library v39.',10,0

strErrAllocImgMem
                dc.b          'Can''t allocate memory for background image.',10,0



                SECTION "dma",data,chip

copperlist      even
;                dc.w    $0207,$fffe ;Required for AGA machines
                dc.w    dmacon,$0020 ;Disable sprites
                dc.w    diwstrt,$2C81   ;DIWSTRT
                dc.w    diwstop,$2CC1   ;DIWSTOP
                dc.w    bplcon2,$0064   ;BPLCON2
                dc.w    ddfstrt,$003C   ;DDFSTRT
                dc.w    ddfstop,$00d4   ;DDFSTOP
                dc.w    bplcon1,$0000   ;BPLCON1
                dc.w    bplcon2,$0000   ;BPL1MOD
                dc.w    bpl2mod,$0000   ;BPL2MOD
                dc.w    bplcon0,$B200   ;BPLCON0
pl1             dc.w    bplpt,$0000     ;BPL1PTH
                dc.w    bplpt+2,$0000   ;BPL1PTL
                dc.w    color,$0f00     ;COLOR00
                dc.w    $7007,$fffe     ;WAIT
                dc.w    color,$0fff     ;COLOR00 -> white
                dc.w    $e007,$fffe     ;WAIT
                dc.w    color,$0f00     ;COLOR00 -> red
;                dc.w    $ff07,$fffe  ;Wait for last ntsc line
                dc.w    dmacon,$8020    ;Enable sprites
                dc.w    $ffff,$fffe     ;Waiting for impossible position

                END

