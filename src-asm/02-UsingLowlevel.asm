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
* History
*   21.07.2019 - Using lowlevel.library
*   20.07.2019 - SimplestCopperlist
*
* Author: Uwe Rosner
*    https://github.com/rosneru
**

;        include   graphics/GfxBase.i
        include   libraries/lowlevel.i

        ;
        ; Stating the used LVOs
        ;
        XREF	_LVOOpenLibrary
        XREF    _LVOCloseLibrary
        XREF    _LVOForbid
        XREF    _LVOPermit
        XREF    _LVOSystemControlA
        XREF    _LVOLoadView
        XREF    _LVOWaitTOF
        XREF    _LVO

*======================================================================
* Initializations
*======================================================================

        move.l  4,a6                ;Use exec.library
        jsr     _LVOForbid(a6)      ;Forbid()

        lea     LowLevelName,a1     ;Opening lowlevel.library
        moveq   #40,d0
        jsr     _LVOOpenLibrary(a6)
        move.l  d0,LowLevelBase     ;Save lowlevel base
        beq.b   Exit

        move.l  LowLevelBase,a6     ;Use lowlevel.library
        lea     TagList1,a1
        jsr     _LVOSystemControlA(a6)
        move.l  d0,System           ;Save TODO
        bne     Exit

        ; Switch off current copper instruction list without smashing
        ; the current screen. The order of LoadView(0) and 2xWaitTOF()
        ; is set by Commodore

        move.l  4,a6                ;Use exec.library
        lea     GfxName,a1          ;Graphics Library name
        moveq   #0,d0
        jsr     _LVOOpenLibrary(a6)  ;OpenLibary()
        move.l  d0,GfxBase          ;Save gfx base
        beq.b   Exit

        move.l  d0,a6                   ;Use graphics.library
        move.l  34(a6),OldView          ;Save current view
        sub.l   a1,a1                   ;Clear a1
        jsr     _LVOLoadView(a6)
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)


*======================================================================
* Main
*======================================================================
        lea     copperlist,a2       ;Load copperlist
        move.l  a2,$dff080          ;Activate copperlist

loop:
        move.l  $dff004,d0          ;WaitTOF (beam)
        and.l   #$fff00,d0
        cmp.l   #$0003000,d0
        bne.s   loop

        btst    #6,$bfe001          ;Left mouse button pressed?
        bne.s   loop

*======================================================================
* Clean up
*======================================================================
Exit:
        move.l  OldView,d0          ;Restore former view
        beq     Exit_1              ;But only if ther was one
        move.l  GfxBase,a6          ;Use graphics.library
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)
        move.l  OldView,a1
        jsr     _LVOLoadView(a6)
        move.l  38(a6),$dff080      ;Start former copper instr. list

Exit_1:
        move.l  GfxBase,d0          ;Load gfx base
        beq     Exit_2              ;Skip if gfx base is empty
        move.l  d0,a1               ;Now gfx base is needed in a1
        move.l  4,a6                ;Use exec.library
        jsr     _LVOCloseLibrary(a6)

Exit_2:
        tst.l   System              ;TODO
        bne     Exit_3
        move.l  LowLevelBase,a6     ;Use lowlevel.library
        lea     TagList2,a1
        jsr     _LVOSystemControlA(a6)

Exit_3:
        move.l  LowLevelBase,d0     ;Load lowlevel base
        beq     Exit_2              ;Skip if lowlevel base is empty
        move.l  d0,a1               ;Now lowlevel base is needed in a1
        move.l  4,a6                ;Use exec.library
        jsr     _LVOCloseLibrary(a6)


Exit_4:
        move.l  4,a6                ;Use exec.library
        jsr     _LVOPermit(a6)
        rts;


*======================================================================
* Data
*======================================================================

LowLevelName
                dc.b    "lowlevel.library",0
                even

LowLevelBase:
                dc.l    0

System:
                dc.l    -1          ; -1 is important; see code refs

GfxName
                dc.b    "graphics.library",0
                even

GfxBase:
                dc.l    0

OldView         dc.l    0

TagList1:
                dc.l    SCON_TakeOverSys,-1     ;TRUE
                dc.l    SCON_AddCreateKeys,1    ;JoyPort1 to RAW keys
                dc.l    TAG_DONE

TagList2:
                dc.l    SCON_TakeOverSys,0      ;FALSE
                dc.l    SCON_RemCreateKeys,1    ;Reset JoyPort1
                dc.l    TAG_DONE


                SECTION customchips,data,chip

copperlist
                dc.w    $008e,$3081 ;DIWSTRT
                dc.w    $0090,$35c1 ;DIWSTOP
                dc.w    $0104,$0064 ;BPLCON2
                dc.w    $0092,$0038 ;DDFSTRT
                dc.w    $0094,$00d0 ;DDFSTOP
                dc.w    $0102,$0000 ;BPLCON1
                dc.w    $0108,$0000 ;BPL1MOD
                dc.w    $010a,$0000 ;BPL2MOD
                dc.w    $0100,$1200 ;BPLCON0
                dc.w    $00e0,$0005 ;BPL1PTH
                dc.w    $00e2,$0000 ;DIWSTOP
                dc.w    $0180,$0000 ;COLOR00
                dc.w    $ffff,$fffe ;Waiting for impossible position

                END
