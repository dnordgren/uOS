.section .exceptions.exit.user, "xa"
call get_global_flag
movi r8, 1
beq r2, r8, mythread_scheduler

mythread_scheduler:
// pass fp and sp to mythread_scheduler (all the context we need)
mov r4, fp
mov r5, sp
call mythread_scheduler
mov sp, r2
mov fp, r3
call reset_global_flag
