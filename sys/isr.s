# @name : isr.s
# @author : rgolani, abmishra, skandalamsri
# @last updated date : 25th September, 2013
.text

.macro set_interrupt index
.global isr\index
isr\index:
            cli
            pushq %rax
            pushq %rcx
            pushq %rdx
            pushq %rsi
            pushq %rdi
            pushq %r8
            pushq %r9
            pushq %r10
            pushq %r11
            movq  %rsp,%rdi
            addq  $72, %rdi
            call isr_handler\index
            popq %r11
            popq %r10
            popq %r9
            popq %r8
            popq %rdi
            popq %rsi
            popq %rdx
            popq %rcx
            popq %rax
            sti
            iretq
.endm

set_interrupt 0
set_interrupt 32
set_interrupt 33
