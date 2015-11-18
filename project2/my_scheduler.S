.section .exceptions.exit.user, "xa"
    call get_global_flag
    movi r8, 1
    bne r2, r8, continue_interrupt
    call reset_global_flag
    mov r4, sp
    mov r5, fp
    call thread_scheduler
    mov sp, r2
    mov fp, r3
continue_interrupt:
