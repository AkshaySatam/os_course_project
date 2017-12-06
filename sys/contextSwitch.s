.section .text
.global context_switch
context_switch:

cli
PUSHF
push %rax
push %rbx
push %rcx
push %rdx
push %rdi
push %rsi
push %r8
push %r9
push %r10
push %r11
push %r12
push %r13
push %r14
push %r15
movq %rsp,8(%rdi)
movq 8(%rsi),%rsp
pop %r15
pop %r14
pop %r13
pop %r12
pop %r11
pop %r10
pop %r9
pop %r8
pop %rsi
pop %rdi
pop %rdx
pop %rcx
pop %rbx
pop %rax
POPF
sti
ret
