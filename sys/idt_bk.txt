# Author: cds
#

.text

######
# load a new IDT
#  parameter 1: address of idtr
.global _x86_64_asm_igdt
_x86_64_asm_igdt:
    lidt (%rdi)
    retq                       # far-return to new cs descriptor ( the retq below )

