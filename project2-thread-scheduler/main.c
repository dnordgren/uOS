/**
 * Operating Syste Kernels, CSCE 351
 * Project 2, Thread Scheduler
 *
 * main.c
 * @author Rees Klintworth
 * @author Derek Nordgren
 *
 * This program schedules and interrupts a series of threads before returning
 * to execute the main thread.
 */

#include <stdlib.h>
#include <malloc.h>
// #include "sys/alt_stdio.h"
// #include "sys/alt_alarm.h"
// #include "alt_types.h"

// compute the number of alarm ticks relative to system ticks per second
// #define ALARMTICKS(x) ((alt_ticks_per_second()*(x))/10)
/* number of threads */
#define NUM_THREADS 12
/* thread runtime */
#define MAX 10000
/* stack size */
#define STACK_SIZE 1000

typedef enum {
    scheduled,
    running,
    blocked,
    finished
} thread_status;

typedef struct stack_context {
    int *sp;
    int *fp;
} stack_context;

typedef struct tcb {
    int thread_id;
    int scheduled_count;
    thread_status status;
    void *blocker_of_thread; /* pointer to thread this thread blocks */
    int joined_thread_count; /* number of threads joined to (blocking) this thread */
    int *sp;
    int *fp;
} tcb;

/**
 * entry point to prototype operating system
 */
void prototype_os();
/**
 * body of work each thread is to complete
 */
void mythread(int thread_id);
/**
 * create new thread
 */
void thread_create(int thread_id, tcb *thread);
/**
 * creates a tcb for the main thread (and places at run_queue[0])
 */
void main_thread_create();
/**
 * cause the main thread to wait on the provided thread
 */
void thread_join(tcb *blocked_thread, tcb *blocking_thread);
/**
 * yield the currently running thread; dispatch a new thread
 */
stack_context thread_scheduler(void *sp, void *fp);
/**
 * callback function for alarm interrupt
 */
//alt_u32 interrupt_handler(void* context);
/**
 * remove completed threads from run queuue
 */
void prune_queue();
/**
 * prioritize threads in run queue
 */
void prioritize_queue();
/**
 * returns the index of the next available (unblocked) thread
 */
int get_next_available_thread_index();
/**
 * deallocate thread workspace
 */
void destroy_thread(tcb *thread);
/**
 * mark thread as finished ("callback"/ra for thread completion)
 */
void finish_thread();
/**
 * disable all interrupts
 */
void disable_interrupts();
/**
 * enable all interrupts
 */
void enable_interrupts();

/* the alarm that will regularly interrupt program execution */
//alt_alarm alarm;
/* used to determine whether interrupt handling should be modified to switch threads */
int global_flag = 0;
/* queue of threads to run; all threads + main */
tcb *run_queue[NUM_THREADS+1];
/* count of threads remaining in queue */
int run_queue_count;
/* the currently running thread */
tcb* current_thread;
/* the index of the currently running thread in the queue */
int current_thread_index;

int main()
{
    /* begin execution of the operating system */
    prototype_os();
    return 0;
}

void mythread(int thread_id)
{
    int i, j, n;
    n = (thread_id % 2 == 0) ? 10 : 15;
    for (i = 0; i < n; i++)
    {
        // alt_printf("This is message %x of thread # %x.\n", i, thread_id);
        for (j = 0; j < MAX; j++);
    }
}

void thread_create(int thread_id, tcb *thread)
{
    /* allocate memory for the thread's workspace and stack */
    tcb *t = (tcb *)malloc(sizeof(tcb) + STACK_SIZE);
    t->thread_id = thread_id;
    t->scheduled_count = 0;
    t->joined_thread_count = 0;
    /* set frame pointer */
    t->fp = t + sizeof(tcb) + STACK_SIZE;
    /* set stack pointer relative to the frame pointer */
    t->sp = t->fp - 19;
    /* set the thread's function to run (ea) */
    *(t->sp + 18) = mythread;
    /* set r4 (argument for mythread) */
    *(t->sp + 5) = thread_id;
    /* enable interrupts (estatus) */
    *(t->sp + 17) = 1;
    /* set the thread's completion function (ra) */
    *(t->sp) = finish_thread;

    /* add thread to run_queue and set status to scheduled */
    run_queue[thread_id] = t;
    run_queue_count++;
    t->status = scheduled;
    thread = t;
}

void main_thread_create()
{
    tcb *t = (tcb *)malloc(sizeof(tcb) + STACK_SIZE);
    t->thread_id = 0;
    t->scheduled_count = 0;
    t->joined_thread_count = 0;
    /* set frame pointer */
    t->fp = t + sizeof(tcb) + STACK_SIZE;
    /* set stack pointer relative to the frame pointer */
    t->sp = t->fp - 19;
    /* enable interrupts (estatus) */
    *(t->sp + 17) = 1;

    /* add thread to run_queue and set status to running */
    run_queue[0] = t;
    t->status = running;
}

void thread_join(tcb *blocked_thread, tcb *blocking_thread)
{
    /* increment blocked thread's joined count and set status to blocked */
    blocked_thread->joined_thread_count++;
    blocked_thread->status = blocked;
    /* tell blocking thread which thread it's blocking */
    blocking_thread->blocker_of_thread = blocked_thread;
}

stack_context thread_scheduler(void *sp, void *fp)
{
    /* save the yielded thread's progress via sp, fp */
    current_thread->sp = (int *)sp;
    current_thread->fp = (int *)fp;

    /* unblock eligible threads; remove completed threads from the queue */
    prune_queue();

    /* run queue still has threads to run (not counting main) */
    if(run_queue_count > 0)
    {
        /* set the next-to-run thread as the current thread */
        current_thread = run_queue[get_next_available_thread_index()];
        /* update next-to-run thread's number of times scheduled count */
        current_thread->scheduled_count++;

        // alt_printf("next thread to run: %x\n", current_thread->thread_id);
    }
    /* all non-main threads have exited */
    else
    {
        // alt_printf("Interrupted by the DE2 timer!\n");
        /* prune queue one last time to unblock main; set main as running thread */
        prune_queue();
        current_thread = run_queue[0];
    }
    /* send the next-to-run thread's stack context back to assembly to be run */
    stack_context context;
    context.sp = current_thread->sp;
    context.fp = current_thread->fp;
    return context;
}

void destroy_thread(tcb *thread)
{
    /* unblock the thread waiting on this thread */
    tcb *thread_to_free = thread->blocker_of_thread;
    if (thread_to_free != NULL) thread_to_free->joined_thread_count--;
    // alt_printf("thread %x destroyed; was scheduled %x times\n", thread->thread_id, thread->scheduled_count);
    free(thread);
}

void finish_thread()
{
    current_thread->status = finished;
}

void prune_queue()
{
    int i;
    for (i = 0; i < NUM_THREADS+1; i++)
    {
        tcb *thread = run_queue[i];
        /* check if blocked threads can be unblocked */
        if (thread != NULL && thread->status == blocked)
        {
            /* unblock threads whose blocking threads have exited */
            if (thread->joined_thread_count == 0) thread->status = scheduled;
        }
        /* check for finished threads that can be destroyed */
        else if (thread != NULL && thread->status == finished)
        {
            /* remove the completed thread from the queue */
            run_queue[i] = NULL;
            run_queue_count--;
            destroy_thread(thread);
        }
    }
}

int get_next_available_thread_index()
{
    tcb *t;
    do {
        /* find the next non-null, unblocked thread to run in the queue */
        if (++current_thread_index > NUM_THREADS) current_thread_index = 0;
        t = run_queue[current_thread_index];
    } while(t == NULL || t->status == blocked);
    return current_thread_index;
}

void prototype_os()
{
    run_queue_count = 0;
    current_thread_index = 0;

    /* create a tcb for the main thread and set it as the running thread */
    main_thread_create();
    current_thread = run_queue[0];

    int i;
    /* create new threads; set their function to execute to mythread */
    for (i = 1; i < NUM_THREADS+1; i++)
    {
        tcb *new_thread;
        thread_create(i, new_thread);
    }

    /* initialize the alarm; set the alarm's callback function */
    //alt_alarm_start(&alarm, alt_ticks_per_second(), interrupt_handler, NULL);

    /* disable interrupts until all threads have been joined to main */
    disable_interrupts();
    for (i = 1; i < NUM_THREADS+1; i++)
    {
        /* join all threads on main (main paused until all threads finish) */
        thread_join(current_thread, run_queue[i]);
    }
    enable_interrupts();

    /* spin until main is unblocked */
    while(run_queue[0]->status == blocked);

    while(1)
    {
        // alt_printf("Hello from uOS!\n");
        for (i = 0; i < 10000; i++);
    }
}

// alt_u32 interrupt_handler(void* context)
// {
//     //alt_printf("Interrupted by timer!\n");
//     /* schedule new thread (global flag observed by my_scheduler.S) */
//     if (run_queue_count > 0)
//     {
//         global_flag = 1;
//     }
//     /* reset the alarm to interrupt next in 0.5 seconds */
//     return ALARMTICKS(5);
// }

void reset_global_flag()
{
    global_flag = 0;
}

int get_global_flag()
{
    return global_flag;
}

void disable_interrupts()
{
    asm("wrctl status, zero");
}

void enable_interrupts()
{
    asm("movi et, 1");
    asm("wrctl status, et");
}
