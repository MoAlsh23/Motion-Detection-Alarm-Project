

;******************************************************************************
; Lab 4 starter code
; ECE 266, Spring 2025
;
; Assembly functions related to buzzer
;
; Created by Zhao Zhang
;******************************************************************************

        ; To include names declared in C
        .cdecls "motion.h"

        .text

;******************************************************************************
; Buzzer signal pin usage: Grove base port J17, Tiva C PC5 (Port C, Pin 5)
;******************************************************************************

; Declare a field to store GPIO_PORTC_BASE (32-bit value) in program memory
_GPIO_PORTC_BASE     .field  GPIO_PORTC_BASE

;******************************************************************************
; C prototype: void buzzOn()
; Turn on the buzzer. It calls GPIOPinWrite() to write 1 to the signal pin.
;******************************************************************************
        .global MotionDetected
        .asmfunc
MotionDetected
        PUSH    {LR}            ; save the return address

        ; Write 1 to the GPIO pin that the buzzer uses:
        ; GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5)
        LDR     r0, _GPIO_PORTC_BASE
        MOV     r1, #GPIO_PIN_4
        MOV     r2, #GPIO_PIN_4
        BL      GPIOPinRead

        POP     {PC}            ; return
        .endasmfunc






