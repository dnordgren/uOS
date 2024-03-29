#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include "alarm_handler.h"
#include "thread_handler.h"
#include "queue.h"

/* The two macros are extremely useful by turning on/off interrupts when atomicity is required */
#define DISABLE_INTERRUPTS() {  \
    asm("wrctl status, zero");  \
}

#define ENABLE_INTERRUPTS() {   \
    asm("movi et, 1");          \
    asm("wrctl status, et");    \
}

/* pointing to the stack/context of main() */
static unsigned int *main_stack_pointer = NULL;

tcb *mythread_create(unsigned int tid, unsigned int stack_size, void (*mythread)(void))
{
    unsigned int *tmp_ptr;
    
    /* allocate a tcb for a thread */
    tcb *thread_pointer;
    
    thread_pointer                      = (tcb *)malloc(sizeof(tcb));
    if (thread_pointer == NULL)
    {
        printf("Unable to allocate space!\n");
        exit(1);
    }
    
    /* initialize the thread's tcb */
    thread_pointer->tid                 = tid;
    thread_pointer->stack               = (unsigned int *)malloc(sizeof(unsigned int) * stack_size);
    if (thread_pointer->stack == NULL)
    {
        printf("Unable to allocate space!\n");
        exit(1);
    }
    thread_pointer->stack_size          = stack_size;
    thread_pointer->stack_pointer       = (unsigned int *)(thread_pointer->stack + stack_size - 19);
    thread_pointer->state               = NEW;
    
    /* initialize the thread's stack */
    tmp_ptr                             = thread_pointer->stack_pointer;
    tmp_ptr[18]                         = (unsigned int)mythread;                               // ea
    tmp_ptr[17]                         = 1;                                                    // estatus
    tmp_ptr[0]                          = (unsigned int)mythread_cleanup;                       // ra
    tmp_ptr[-1]                         = (unsigned int)(thread_pointer->stack + stack_size);   // fp
           
    return thread_pointer;
}

/* NEW ----> READY */
void mythread_start(tcb *thread_pointer)
{
    thread_pointer->state = READY;
}

/* READY --push into--> readyQ */
void mythread_join(tcb *thread_pointer)
{
    enqueue((void *)thread_pointer);
}

/* RUNNING ----> BLOCKED */
void mythread_block(tcb *thread_pointer)
{
    thread_pointer->state = BLOCKED;
}

/* RUNNING ----> TERMINATED */
void mythread_terminate(tcb *thread_pointer)
{
    thread_pointer->state = TERMINATED;
}

void *mythread_schedule(void *context)
{
    if (getQsize() > 0)
    {
        if (current_running_thread != NULL)
        {
        	if (current_running_thread->state != BLOCKED)
        	{
        		current_running_thread->state = READY;
        	}
            current_running_thread->stack_pointer = (unsigned int *)context;
            enqueue(current_running_thread);
        }
        else if (main_stack_pointer == NULL)
        {
            main_stack_pointer = (unsigned int *)context;
        }
        current_running_thread = (tcb *)dequeue();
        current_running_thread->state = RUNNING;
        
        context = (void *)(current_running_thread->stack_pointer);
    }
    else if (current_running_thread==NULL && main_stack_pointer!=NULL)
    {        
        context = (void *)main_stack_pointer;
    }

    return context;
}

unsigned int mythread_isQempty()
{
    return (getQsize() == 0) && (current_running_thread == NULL);
}

void mythread_cleanup()
{
    DISABLE_INTERRUPTS();
    printf("terminating thread %u\n", current_running_thread->tid);
    mythread_terminate(current_running_thread);
    free(current_running_thread->stack);
    free(current_running_thread);
    current_running_thread = NULL;
    ENABLE_INTERRUPTS();
    while(1);
}
