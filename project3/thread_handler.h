/* 
    Name:   thread_handler.h
    Author: Dongyuan Zhan
    Date:   11/20/2010

Description:
    Functions used to manage threads
*/

#ifndef THREAD_HANDLER_H_
#define THREAD_HANDLER_H_

#include <stdlib.h>

/* possible thread states */
enum tstate {NEW, READY, RUNNING, BLOCKED, TERMINATED, NUM_TSTATES};

/* thread control block */
typedef struct
{
    unsigned int tid;
    unsigned int *stack;
    unsigned int stack_size;
    unsigned int *stack_pointer;
    enum tstate  state;
} tcb;

/* the current running thread */
extern tcb *current_running_thread;

/* declaration */
tcb *mythread_create(unsigned int tid, unsigned int stack_size, void (*mythread)(void));

void mythread_start(tcb *thread_pointer);

void mythread_join(tcb *thread_pointer);

void mythread_block(tcb *thread_pointer);

void mythread_terminate(tcb *thread_pointer);

void *mythread_schedule(void *context);

unsigned int mythread_isQempty();

void mythread_cleanup();

#endif /* THREAD_HANDLER_H_ */
