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
*   17.08.2019 - Started adding the color cycling routine
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
        move.l  #10800,d0           ;Size of needed memory
        move.l  #MEMF_CHIP,d1       ;It must be Chip memory
        or.l    #MEMF_CLEAR,d1      ;New memory should be cleared
        jsr     _LVOAllocVec(a6)
        move.l  #strErrAllocImgMem,d1 ;The error message if it failed
        move.l  d0,picture          ;Save ptr of allocated memory
        beq     Exit_err            ;No memory has been reserved

        ;
        ; Switch off current copper list without smashing the current
        ; screen. The order of LoadView(0) and 2xWaitTOF() is set by
        ; Commodore
        ;
        movea.l _GfxBase,a6             ;Use graphics.library
        move.l  gb_ActiView(a6),oldview ;Save current view

        sub.l   a1,a1                   ;Clear a1 to display no view
        jsr     _LVOLoadView(a6)
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)

        jsr     _LVOOwnBlitter(a6)

        movea.l _SysBase,a6
        jsr     _LVOForbid(a6)

        move.l  #$0020,$dff096      ;Disable sprites

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

        ; Create the copperlist dynamicaly
        bsr     initcopper

        ; Activate the copperlist
        lea     _custom,a1
        move.l  #copperlist,cop1lc(a1)

loop:
        btst    #CIAB_GAMEPORT0,_ciaa
        bne     loop


*======================================================================
* Clean up
*======================================================================


        move.l  #$8020,$dff096      ;Enable sprites

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
        rts;


; NOTE: a1 must be loaded with the address of a error text before!
Exit_err:
        movea.l _DOSBase,a6
        move.l  0,d2
        jsr     _LVOVPrintf(a6)
        bra     Exit


*======================================================================
* Sub
*======================================================================

initcopper:
        lea     cins,a0
        lea     copcol,a1
        move.l  #99,d0
        move.l  #$6007,d1

coppercopy:
        move.w  d1,(a0)+
        move.w  #$fffe,(a0)+
        move.w  #$0180,(a0)+
        cmp     #$ffff,(a1)
        bne.s   outer
        lea     copcol,a1

outer:
        move.w  (a1)+,(a0)+
        add     #$0100,d1
        dbf     d0,coppercopy
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

copcol
                dc.w    $000,$111,$222,$333,$444,$555,$666,$777,$888,$999
                dc.w    $aaa,$bbb,$ccc,$ddd,$eee,$fff,$eee,$ddd,$ccc,$bbb
                dc.w    $aaa,$999,$888,$777,$666,$555,$444,$333,$222,$111
                dc.w    $ffff


copperlist
;                dc.w    $0207,$fffe ;Required for AGA machines
                dc.w    $008e,$3081 ;DIWSTRT
                dc.w    $0090,$35c1 ;DIWSTOP
                dc.w    $0104,$0064 ;BPLCON2
                dc.w    $0092,$0038 ;DDFSTRT
                dc.w    $0094,$00d0 ;DDFSTOP
                dc.w    $0102,$0000 ;BPLCON1
                dc.w    $0108,$0000 ;BPL1MOD
                dc.w    $010a,$0000 ;BPL2MOD
                dc.w    $0100,$1200 ;BPLCON0
pl1             dc.w    $00e0,$0000 ;BPL1PTH
                dc.w    $00e2,$0000 ;BPL1PTL
                dc.w    $0180,$0000 ;COLOR00
cins            blk.w   400,0
                dc.w    $0180,$0000 ;COLOR00 -> red
;                dc.w    $ff07,$fffe ;Wait for last ntsc line
                dc.w    $ffff,$fffe ;Waiting for impossible position 

                END

