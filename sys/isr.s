#author: cds
#

.text

######
# load a new IDT
#  parameter 1: address of gdtr
#  parameter 2: new code descriptor offset
#  parameter 3: new data descriptor offset

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
set_interrupt 1
set_interrupt 2
set_interrupt 3
set_interrupt 4
set_interrupt 5
set_interrupt 6
set_interrupt 7
set_interrupt 8
set_interrupt 9
set_interrupt 10
set_interrupt 11
set_interrupt 12
set_interrupt 13
set_interrupt 14
set_interrupt 15
set_interrupt 16
set_interrupt 17
set_interrupt 18
set_interrupt 19
set_interrupt 20
set_interrupt 21
set_interrupt 22
set_interrupt 23
set_interrupt 24
set_interrupt 25
set_interrupt 26
set_interrupt 27
set_interrupt 28
set_interrupt 29
set_interrupt 30
set_interrupt 31
set_interrupt 32
set_interrupt 33
set_interrupt 128
