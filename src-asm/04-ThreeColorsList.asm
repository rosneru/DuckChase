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
*   22.07.2019 - Modified copper list, displaying three colored blocks
*   21.07.2019 - Dynamic allocation of chip memory for the view
*   21.07.2019 - Using lowlevel.library
*   20.07.2019 - SimplestCopperlist
*
* Author: Uwe Rosner
*    https://github.com/rosneru
**

        include exec/memory.i
        include libraries/lowlevel.i

        ;
        ; Stating the used LVOs
        ;
        XREF	_LVOOpenLibrary
        XREF    _LVOCloseLibrary
        XREF    _LVOAllocVec
        XREF    _LVOFreeVec
        XREF    _LVOSystemControlA
        XREF    _LVOLoadView
        XREF    _LVOWaitTOF

*======================================================================
* Initializations
*======================================================================

        move.l  4,a6                ;Use exec.library

        ;
        ; Open graphics.library
        ;
        move.l  4,a6
        lea     GfxName,a1
        moveq   #0,d0
        jsr     _LVOOpenLibrary(a6)
        move.l  d0,GfxBase
        beq     Exit

        ;
        ; Open lowlevel.library
        ;
        lea     LowLevelName,a1
        moveq   #40,d0
        jsr     _LVOOpenLibrary(a6)
        move.l  d0,LowLevelBase     ;Save lowlevel base
        beq     Exit

        ;
        ; Allocate memory for picture
        ;
        move.l  #10800,d0           ;Size of needed memory
        move.l  #MEMF_CHIP,d1       ;It must be Chip memory
        or.l    #MEMF_CLEAR,d1      ;New memory should be cleared
        jsr     _LVOAllocVec(a6)
        move.l  d0,picture          ;Save ptr of allocated memory
        tst.l   d0
        beq     Exit                ;No memory has been reserved

        ;
        ; Takeover the system in a compatible way
        ;
        move.l  LowLevelBase,a6
        lea     TagList1,a1
        jsr     _LVOSystemControlA(a6)
        move.l  d0,System           ;Result is 0 on success
        bne     Exit

*======================================================================
* Main
*======================================================================

        ;
        ; Switch off current copper instruction list without smashing
        ; the current screen. The order of LoadView(0) and 2xWaitTOF()
        ; is set by Commodore
        ;
        move.l  GfxBase,a6          ;Use graphics.library
        move.l  34(a6),OldView      ;Save current view
        sub.l   a1,a1               ;Clear a1 to display no view
        jsr     _LVOLoadView(a6)
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)

        ;
        ; Load address of allocated memory for the picture
        ;
        move.l  #picture,d0
        move.w  d0,pl1+6
        swap    d0
        move.w  d0,pl1+2
        move.l  #copperlist,$dff080 ;Activate copper list
loop:
        move.l  $dff004,d0          ;WaitTOF (beam)
        and.l   #$fff00,d0
        cmp.l   #$0003000,d0
        bne.s   loop

        btst    #6,$bfe001          ;Left mouse button pressed?
        bne.s   loop

RestoreView:
        move.l  OldView,d0          ;Restore former view
        beq     Exit_1              ;But only if ther was one
        move.l  GfxBase,a6          ;Use graphics.library
        jsr     _LVOWaitTOF(a6)
        jsr     _LVOWaitTOF(a6)
        move.l  OldView,a1
        jsr     _LVOLoadView(a6)
        move.l  38(a6),$dff080      ;Start former copper instr. list

*======================================================================
* Clean up
*======================================================================

Exit:
        ; Restore system control if it was acquired sucessfully
        tst.l   System
        bne     Exit_1
        move.l  LowLevelBase,a6
        lea     TagList2,a1
        jsr     _LVOSystemControlA(a6)

Exit_1:
        ; Free memory if it was allocated successfully
        move.l  4,a6
        move.l  picture,d0
        tst.l   d0
        beq     Exit_2
        move.l  d0,a1
        jsr     _LVOFreeVec(a6)

Exit_2:
        ; Close lowlevel.library
        move.l  4,a6
        move.l  LowLevelBase,d0     ;LibBase needed in d-reg for verify
        beq     Exit_3
        move.l  d0,a1               ;LibBase needed in a1 for closing
        jsr     _LVOCloseLibrary(a6)

Exit_3:
        ; Close graphics.library
        move.l  4,a6                ;Use exec.library
        move.l  GfxBase,d0          ;LibBase needed in d-reg for verify
        beq     Exit_4
        move.l  d0,a1               ;LibBase needed in a1 for closing
        jsr     _LVOCloseLibrary(a6)


Exit_4:
        rts;


*======================================================================
* Data
*======================================================================

LowLevelName:
                dc.b    "lowlevel.library",0
                even

LowLevelBase:
                ds.l    1

System:
                dc.l    -1          ; -1 is important; see code refs

GfxName:
                dc.b    "graphics.library",0
                even

GfxBase:
                ds.l    1

OldView:
                dc.l    0

TagList1:
                dc.l    SCON_TakeOverSys,-1     ;TRUE
                dc.l    SCON_AddCreateKeys,1    ;JoyPort1 to RAW keys
                dc.l    TAG_DONE

TagList2:
                dc.l    SCON_TakeOverSys,0      ;FALSE
                dc.l    SCON_RemCreateKeys,1    ;Reset JoyPort1
                dc.l    TAG_DONE

picture:
                ds.l    1

                SECTION customchips,data,chip

copperlist      CNOP    0,4
                dc.w    $008e,$3081 ;DIWSTRT
                dc.w    $0090,$35c1 ;DIWSTOP
                dc.w    $0104,$0064 ;BPLCON2
                dc.w    $0092,$0038 ;DDFSTRT
                dc.w    $0094,$00d0 ;DDFSTOP
                dc.w    $0102,$0000 ;BPLCON1
                dc.w    $0108,$0000 ;BPL1MOD
                dc.w    $010a,$0000 ;BPL2MOD
                dc.w    $0100,$1200 ;BPLCON0
pl1:            dc.w    $00e0,$0005 ;BPL1PTH
                dc.w    $00e2,$0000 ;BPL1PTL

                dc.w    $0180,$0f00 ;COLOR00 -> red
                dc.w    $7007,$fffe ;WAIT
                dc.w    $0180,$0fff ;COLOR00 -> white
                dc.w    $e007,$fffe ;WAIT
                dc.w    $0180,$0f00 ;COLOR00 -> red
                dc.w    $ffff,$fffe ;Waiting for impossible position


                END
