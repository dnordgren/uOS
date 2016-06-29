/**
 * Operating System Kernels, CSCE 351 Fall 2015
 * Project 2, Thread Scheduler
 *
 * tcb.h
 * @author Rees Klintworth
 * @author Derek Nordgren
 *
 * This class provides various functions related to create, managing and
 * destroying thread control blocks for thread instances.
 */

#ifndef __TCB_H__
#define __TCB_H__

#include <stdlib.h>

/* stack size */
#define STACK_SIZE 1000

/* represents a thread's status in the queue */
typedef enum {
    scheduled,
    running,
    blocked,
    finished
} thread_status;

/* used to pass frame and stack pointers to/from assembly */
typedef struct stack_context {
    int *sp;
    int *fp;
} stack_context;

/* thread control block */
typedef struct tcb {
    int thread_id;
    int scheduled_count; /* the number of times the thread has been scheduled */
    thread_status status; /* thread's status in queue */
    void *blocker_of_thread; /* pointer to thread this thread blocks */
    int joined_thread_count; /* number of threads joined to (blocking) this thread */
    int *sp; /* stack pointer */
    int *fp; /* frame pointer */
} tcb;

/**
 * create new thread; set its function to run as the provided function
 */
void thread_create(int thread_id, tcb *thread, void (*run), void (*callback));
/**
 * creates a tcb for the main thread (and places at run_queue[0])
 */
void main_thread_create(tcb *thread);
/**
 * cause the main thread to wait on the provided thread
 */
void thread_join(tcb *blocked_thread, tcb *blocking_thread);
/**
 * deallocate thread workspace
 */
void destroy_thread(tcb *thread);

#endif /* __TCB_H__ */
