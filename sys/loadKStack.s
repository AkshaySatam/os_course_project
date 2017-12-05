.section .text
.global loadKStack
loadKStack:

push %rcx
movq 8(%rdi),%rsp
ret

