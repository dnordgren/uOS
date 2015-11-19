/**
 * Operating System Kernels, CSCE 351 Fall 2015
 * Project 2, Thread Scheduler
 *
 * tcb.c
 * @author Rees Klintworth
 * @author Derek Nordgren
 *
 * This class provides various functions related to create, managing and
 * destroying thread control blocks for thread instances.
 */

#include "tcb.h"

void thread_create(int thread_id, tcb *thread, void (*run), void (*callback))
{
    /* allocate memory for the thread's workspace and stack */
    tcb *t = (tcb *)malloc(sizeof(tcb) + STACK_SIZE);
    t->thread_id = thread_id;
    t->scheduled_count = 0;
    t->joined_thread_count = 0;
    /* set frame pointer */
    t->fp = (int *)(t + sizeof(tcb) + STACK_SIZE);
    /* set stack pointer relative to the frame pointer */
    t->sp = t->fp - 19;
    /* set the thread's function to run (ea) */
    *(t->sp + 18) = *((int *)run);
    /* set r4 (argument for mythread) */
    *(t->sp + 5) = thread_id;
    /* enable interrupts (estatus) */
    *(t->sp + 17) = 1;
    /* set the thread's completion function (ra) */
    t->sp = (int *)callback;
    t->status = scheduled;
    thread = t;
}

void main_thread_create(tcb *thread)
{
    tcb *t = (tcb *)malloc(sizeof(tcb) + STACK_SIZE);
    t->thread_id = 0;
    t->scheduled_count = 0;
    t->joined_thread_count = 0;
    /* set frame pointer */
    t->fp = (int *)(t + sizeof(tcb) + STACK_SIZE);
    /* set stack pointer relative to the frame pointer */
    t->sp = t->fp - 19;
    /* enable interrupts (estatus) */
    *(t->sp + 17) = 1;

    /* add thread to run_queue and set status to running */
    thread = t;
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

void destroy_thread(tcb *thread)
{
    /* unblock the thread waiting on this thread */
    tcb *thread_to_free = thread->blocker_of_thread;
    if (thread_to_free != NULL) thread_to_free->joined_thread_count--;
    alt_printf("thread %x destroyed; was scheduled %x times\n", thread->thread_id, thread->scheduled_count);
    free(thread);
}
