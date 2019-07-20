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
*   20.03.2019 - SimplestCopperlist
*
* Author: Uwe Rosner
*    https://github.com/rosneru
**
        include   exec/exec_lib.i
        include   exec/execbase.i
        include   exec/io.i
        include   exec/memory.i
        include   exec/ports.i
        include   dos/dos_lib.i
        include   dos/dos.i
        include   dos/dosextens.i
        include   devices/timer.i
        include   lvo/timer.i
        include   lvo/mathieeesingbas.i

_SYSBase = 4

OS_VERSION = 40                      ;Minimal requierd OS is 3.1


*======================================================================
* Initializations
*======================================================================
init:
        move.w  #$4000,$dff09a      ;Forbid interrupts, disable the OS
        move.w  #$0020,$dff096      ;Disable sprites


*======================================================================
* Main
*======================================================================
main:
        move.l  #copperlist,$dff084 ;Activate copperlist

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
cleanup:
        move.w   #$c000,$dff09a     ;Allow interrupts
        rts

*======================================================================
* Data
*======================================================================

copperlist      dc.w    $008e,$3081 ;DIWSTRT
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
