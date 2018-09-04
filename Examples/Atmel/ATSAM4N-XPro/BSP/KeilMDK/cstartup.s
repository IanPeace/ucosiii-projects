;/*****************************************************************************
; * @file     startup_SAM4N.s
; * @brief    CMSIS Cortex-M4 Core Device Startup File for
; *           Atmel SAM4N Device Series
; * @version  V1.01
; * @date     21. November 2014
; *
; * @note
; * Copyright (C) 2013 - 2014 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                IMPORT  BSP_IntHandlerSUPC
                IMPORT  BSP_IntHandlerRSTC
                IMPORT  BSP_IntHandlerRTC
                IMPORT  BSP_IntHandlerRTT
                IMPORT  BSP_IntHandlerWDT
                IMPORT  BSP_IntHandlerPMC
                IMPORT  BSP_IntHandlerEFC
                IMPORT  BSP_IntHandlerRSVD7
                IMPORT  BSP_IntHandlerUART0
                IMPORT  BSP_IntHandlerUART1
                IMPORT  BSP_IntHandlerUART2
                IMPORT  BSP_IntHandlerPIOA
                IMPORT  BSP_IntHandlerPIOB
                IMPORT  BSP_IntHandlerPIOC
                IMPORT  BSP_IntHandlerUSART0
                IMPORT  BSP_IntHandlerUSART1
                IMPORT  BSP_IntHandlerUART3
                IMPORT  BSP_IntHandlerUSART2
                IMPORT  BSP_IntHandlerRSVD18
                IMPORT  BSP_IntHandlerTWI0
                IMPORT  BSP_IntHandlerTWI1
                IMPORT  BSP_IntHandlerSPI
                IMPORT  BSP_IntHandlerTWI2
                IMPORT  BSP_IntHandlerTC0
                IMPORT  BSP_IntHandlerTC1
                IMPORT  BSP_IntHandlerTC2
                IMPORT  BSP_IntHandlerTC3
                IMPORT  BSP_IntHandlerTC4
                IMPORT  BSP_IntHandlerTC5
                IMPORT  BSP_IntHandlerADC
                IMPORT  BSP_IntHandlerDACC
                IMPORT  BSP_IntHandlerPWM
                IMPORT  OS_CPU_PendSVHandler
                IMPORT  OS_CPU_SysTickHandler

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                            ; 000 Initial stack pointer.
                DCD     App_Reset_ISR                           ; 001 Initial program counter.
                DCD     App_NMI_ISR                             ; 002 Non-maskable interrupt.
                DCD     App_Fault_ISR                           ; 003 Hard fault exception.
                DCD     App_Spurious_ISR                        ; 004 Reserved interrupt 4.
                DCD     App_BusFault_ISR                        ; 005 Bus fault exception.
                DCD     App_UsageFault_ISR                      ; 006 Usage fault exception.
                DCD     App_Spurious_ISR                        ; 007 Reserved interrupt 7.
                DCD     App_Spurious_ISR                        ; 008 Reserved interrupt 8.
                DCD     App_Spurious_ISR                        ; 009 Reserved interrupt 9.
                DCD     App_Spurious_ISR                        ; 010 Reserved interrupt 10.
                DCD     App_Spurious_ISR                        ; 011 A supervisor call exception.
                DCD     App_Spurious_ISR                        ; 012 Debug Monitor.
                DCD     App_Spurious_ISR                        ; 013 Reserved interrupt 13.
                
                DCD     OS_CPU_PendSVHandler                    ; 014 PendSV exception.
                DCD     OS_CPU_SysTickHandler                   ; 015 SysTick Interrupt.

                ; Configurable interrupts
                DCD     BSP_IntHandlerSUPC                      ; 016 IRQ[  0] Supply Controller interrupt
                DCD     BSP_IntHandlerRSTC                      ; 017 IRQ[  1] Reset Controller interrupt
                DCD     BSP_IntHandlerRTC                       ; 018 IRQ[  2] Real Time Clock interrupt
                DCD     BSP_IntHandlerRTT                       ; 019 IRQ[  3] Real Time Timer interrupt
                DCD     BSP_IntHandlerWDT                       ; 020 IRQ[  4] Watchdog Timer interrupt
                DCD     BSP_IntHandlerPMC                       ; 021 IRQ[  5] Power Management Controller interrupt
                DCD     BSP_IntHandlerEFC                       ; 022 IRQ[  6] Enhanced Flash Controller interrupt
                DCD     BSP_IntHandlerRSVD7
                DCD     BSP_IntHandlerUART0                     ; 024 IRQ[  8] UART 0 interrupt
                DCD     BSP_IntHandlerUART1                     ; 025 IRQ[  9] UART 1 interrupt
                DCD     BSP_IntHandlerUART2                     ; 026 IRQ[ 10] UART 2 interrupt
                DCD     BSP_IntHandlerPIOA                      ; 027 IRQ[ 11] Parallel I/O Controller A interrupt
                DCD     BSP_IntHandlerPIOB                      ; 028 IRQ[ 12] Parallel I/O Controller B interrupt
                DCD     BSP_IntHandlerPIOC                      ; 029 IRQ[ 13] Parallel I/O Controller C interrupt
                DCD     BSP_IntHandlerUSART0                    ; 030 IRQ[ 14] USART 0 interrupt
                DCD     BSP_IntHandlerUSART1                    ; 031 IRQ[ 15] USART 1 interrupt
                DCD     BSP_IntHandlerUART3                     ; 032 IRQ[ 16] UART 3 interrupt
                DCD     BSP_IntHandlerUSART2                    ; 033 IRQ[ 17] USART 2 interrupt
                DCD     BSP_IntHandlerRSVD18
                DCD     BSP_IntHandlerTWI0                      ; 035 IRQ[ 19] Two Wire Interface 0 interrupt
                DCD     BSP_IntHandlerTWI1                      ; 036 IRQ[ 20] Two Wire Interface 1 interrupt
                DCD     BSP_IntHandlerSPI                       ; 037 IRQ[ 21] Serial Peripheral Interface interrupt
                DCD     BSP_IntHandlerTWI2                      ; 038 IRQ[ 22] Two Wire Interface 2 interrupt
                DCD     BSP_IntHandlerTC0                       ; 039 IRQ[ 23] Timer/Counter 0 interrupt
                DCD     BSP_IntHandlerTC1                       ; 040 IRQ[ 24] Timer/Counter 1 interrupt
                DCD     BSP_IntHandlerTC2                       ; 041 IRQ[ 25] Timer/Counter 2 interrupt
                DCD     BSP_IntHandlerTC3                       ; 042 IRQ[ 26] Timer/Counter 3 interrupt
                DCD     BSP_IntHandlerTC4                       ; 043 IRQ[ 27] Timer/Counter 4 interrupt
                DCD     BSP_IntHandlerTC5                       ; 044 IRQ[ 28] Timer/Counter 5 interrupt
                DCD     BSP_IntHandlerADC                       ; 045 IRQ[ 29] Analog To Digital Converter interrupt
                DCD     BSP_IntHandlerDACC                      ; 046 IRQ[ 30] Digital To Analog Converter interrupt
                DCD     BSP_IntHandlerPWM                       ; 047 IRQ[ 31] Pulse Width Modulation interrupt
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

App_Reset_ISR   PROC
                EXPORT  App_Reset_ISR             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

App_NMI_ISR\
                PROC
                EXPORT  App_NMI_ISR               [WEAK]
                B       .
                ENDP
App_Fault_ISR\
                PROC
                EXPORT  App_Fault_ISR             [WEAK]
                B       .
                ENDP
App_BusFault_ISR\
                PROC
                EXPORT  App_BusFault_ISR          [WEAK]
                B       .
                ENDP
App_UsageFault_ISR\
                PROC
                EXPORT  App_UsageFault_ISR        [WEAK]
                B       .
                ENDP
App_Spurious_ISR\
                PROC
                EXPORT  App_Spurious_ISR          [WEAK]
                B       .
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
