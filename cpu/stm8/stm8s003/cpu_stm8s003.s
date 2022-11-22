
; External reference
        EXTERN  OSStackPtr
        
        EXTERN  OSTimerInt
        EXTERN  OSSoftInt
        
        EXTERN ?pop_l0
        EXTERN ?pop_l1
        EXTERN ?push_l0
        EXTERN ?push_l1


; OS core function
        PUBLIC  OSSwCtxInt
        
        PUBLIC  TIM4_UPD_OVF_IRQHandler
        PUBLIC  TRAP_IRQHandler
        PUBLIC  _interrupt_25
        PUBLIC  _interrupt_1
        
        PUBLIC  OSPushVirtual
        PUBLIC  OSPopVirtual
        PUBLIC  OSBootStackSwitch

        SECTION `.far_func.text`:CODE:REORDER:NOROOT(0)
OSPushVirtual:
        POPW      X
        POP       A
        PUSH      S:0x00
        PUSH      S:0x01
        PUSH      S:0x02
        PUSH      S:0x03
        PUSH      S:0x04
        PUSH      S:0x05
        PUSH      S:0x06
        PUSH      S:0x07
        PUSH      S:0x08
        PUSH      S:0x09
        PUSH      S:0x0A
        PUSH      S:0x0B
        PUSH      S:0x0C
        PUSH      S:0x0D
        PUSH      S:0x0E
        PUSH      S:0x0F
        PUSH      S:0x10
        PUSH      A
        PUSHW     X
        RETF
        
        SECTION `.far_func.text`:CODE:REORDER:NOROOT(0)
OSPopVirtual:
        POPW      X
        POP       A
        POP       S:0x10
        POP       S:0x0F
        POP       S:0x0E
        POP       S:0x0D
        POP       S:0x0C
        POP       S:0x0B
        POP       S:0x0A
        POP       S:0x09
        POP       S:0x08
        POP       S:0x07
        POP       S:0x06
        POP       S:0x05
        POP       S:0x04
        POP       S:0x03
        POP       S:0x02
        POP       S:0x01
        POP       S:0x00
        PUSH      A
        PUSHW     X
        RETF

        SECTION `.far_func.text`:CODE:REORDER:NOROOT(0)
OSSwCtxInt:
        LDW   X, OSStackPtr             ; Load SP from memory
        LDW   SP, X
        POP   A                         ; Ignore ISR call pc
        POP   A
        POP   A
        CALLF   OSPopVirtual
        ;CALLF     L:?pop_l1             ; IAR auto added some code in ISR, please refer to STM8 Errata
        ;CALLF     L:?pop_l0
        IRET                            ; Restore reg from stack

        SECTION `.far_func.text`:CODE:REORDER:NOROOT(0)
TIM4_UPD_OVF_IRQHandler:
_interrupt_25:
        BRES      L:0x5344, #0x0
        CALLF     OSPushVirtual
        CALLF     OSTimerInt
        CALLF     OSPopVirtual
        IRET

        SECTION `.far_func.text`:CODE:REORDER:NOROOT(0)
TRAP_IRQHandler:
_interrupt_1:
        CALLF     OSPushVirtual
        CALLF     OSSoftInt
        CALLF     OSPopVirtual
        IRET
        
        SECTION `.far_func.text`:CODE:REORDER:NOROOT(0)
OSBootStackSwitch:
        POPW      X
        POPW      Y
        SUB       SP, #0x20
        PUSHW     Y
        PUSHW     X
        RETF
        
        