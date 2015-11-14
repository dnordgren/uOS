.section .exceptions.exit.user, "xa"
call get_global_flag
movi r8, 1
beq r2, r8, thread_scheduler

thread_scheduler:
mov r4, sp
mov r5, fp
call thread_scheduler
mov sp, r4
mov fp, r5
call reset_global_flag
