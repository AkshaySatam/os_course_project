.section .text
.global switchToRing3
switchToRing3:
cli;
movq %rsp,8(%rdi);
movq 16(%rdi),%rax; //User space rsp
push $0x23; //stack segment selector
push %rax; //push rax on stack
PUSHF;//eflags
pop %rax; //Get EFLAGS back into EAX.
or  $0x200,%rax; //Set the IF flag.
push %rax; //Push the new EFLAGS value back onto the stack.
push $0x2b; //code segment selector
push 24(%rdi);//the addres of the function we want to execute
iretq;

