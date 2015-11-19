/**
 * Operating System Kernels, CSCE 351 Fall 2015
 * Project 2, Thread Scheduler
 *
 * main.c
 * @author Rees Klintworth
 * @author Derek Nordgren
 *
 * This program schedules and interrupts a series of threads before returning
 * to execute the main thread.
 */

#include "tcb.h"

// #include "sys/alt_stdio.h"
// #include "sys/alt_alarm.h"
// #include "alt_types.h"

/* compute the number of alarm ticks relative to system ticks per second */
// #define ALARMTICKS(x) ((alt_ticks_per_second()*(x))/10)
/* number of threads */
#define NUM_THREADS 12
/* thread runtime */
#define MAX 10000

/**
 * entry point to prototype operating system
 */
void prototype_os();
/**
 * body of work each thread is to complete
 */
void mythread(int thread_id);
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
 * returns the index of the next available (unblocked) thread
 */
int get_next_available_thread_index();
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

void prototype_os()
{
    run_queue_count = 0;
    current_thread_index = 0;

    /* create a tcb for the main thread and set it as the running thread */
    tcb *main_thread;
    main_thread_create(main_thread);
    run_queue[0] = main_thread;
    current_thread = run_queue[0];

    int i;
    /* create new threads; set their function to execute to mythread */
    for (i = 1; i < NUM_THREADS+1; i++)
    {
        tcb *new_thread;
        thread_create(i, new_thread, mythread, finish_thread);
        run_queue[i] = new_thread;
        run_queue_count++;
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

void finish_thread()
{
    current_thread->status = finished;
}

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
//     asm("wrctl status, zero");
}

void enable_interrupts()
{
//     asm("movi et, 1");
//     asm("wrctl status, et");
}

int main()
{
    /* begin execution of the operating system */
    prototype_os();
    return 0;
}
