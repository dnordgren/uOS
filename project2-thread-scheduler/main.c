/*
 * Project 2, Thread Scheduler
 * main.c
 * @author Rees Klintworth
 * @author Derek Nordgren
 *
 * This program schedules and interrupts a series of threads before returning
 * to execute the main thread.
 */

#include <stdlib.h>
#include <setjmp.h>
#include <malloc.h>
#include "sys/alt_stdio.h"
#include "sys/alt_alarm.h"
#include "alt_types.h"

// compute the number of alarm ticks relative to system ticks per second
#define ALARMTICKS(x) ((alt_ticks_per_second()*(x))/10)
// define the number of threads
#define NUM_THREADS 12
// define thread runtime
#define MAX 10000
#define STACK_SIZE 4096
// disable interrupts
#define DISABLE_INTERRUPTS() { \
	asm("wrct1 status, zero"); \
}
// enable interrupts
#define ENABLE_INTERRUPTS() { \
	asm("movi et, 1");        \
	asm("wrct1 status, et");  \
}

typedef enum {
	scheduled,
	running,
	finished
} thread_status;

typedef struct tcb {
	int thread_id;
	int scheduled_count;
	thread_status status;
	void *stack_pointer;
	void *frame_pointer;
	void (*function)(void *arg);
	void *arg;
	jmp_buf buf;
} tcb;

// entry point to prototype operating system
void prototype_os();
void mythread(int thread_id);
void mythread_create(int thread_id, tcb *thread, void(*f)(void *arg), void *arg);
void * mythread_scheduler(void *context);
// callback function for alarm interrupt
alt_u32 interrupt_handler(void* context);
// helper function for pruning run queue of completed threads
void prune_queue();
// helper function for prioritizing threads in run queue
void prioritize_queue();
// helper function used to deallocate tcb for a thread instance
void destroy_thread(tcb *thread);

// the alarm that will regularly interrupt program execution
alt_alarm alarm;

// the thread to return to once execution of the current thread has finished
tcb *ret;

int global_flag = 0;

tcb *run_queue[NUM_THREADS];
int run_queue_count;
int current_thread_id;

int main()
{
	// begin execution of the operating system
	prototype_os();
	return 0;
}

void mythread(int thread_id)
{
	int i, j, n;
	n = (thread_id % 2 == 0) ? 10 : 15;
	for (i = 0; i < n; i++)
	{
		alt_printf("This is message %d of thread # %d.\n", i, thread_id);
		for (j = 0; j < MAX; j++);
	}
}

void mythread_create(int thread_id, tcb *thread, void(*f)(void *arg), void *arg)
{
	// TODO set ea to be the function we want to return
	// alt_exception_entry tells us the location of ea 72(sp)
	// which is sp + 72/4

	// allocate memory for the thread's workspace
	tcb *t = (tcb *)malloc(sizeof(tcb));
	t->thread_id = thread_id;
	// allocate memory for the thread's stack
	// TODO malloc stack space all at once to assure contiguous memory
	void *stack = memalign(8, STACK_SIZE);
	t->frame_pointer = stack;
	// set stack pointer
	t->stack_pointer = stack + STACK_SIZE;
	// set the thread's function to run
	t->function = f;

	// add thread to run_queue
	run_queue[thread_id] = t;
	run_queue_count++;
	// thread has been added to queue; set its status to scheduled
	t->status = scheduled;
	thread = t;
}

void mythread_join()
{
	// suspend main thread while other threads are running
	// once other threads have died, resume main
	// save a copy of the main thread to return to
	mythread_create(-1, ret, NULL, NULL);
}

void thread_yield()
{
	// save the context of the currently-running thread
	if(!setjmp(run_queue[current_thread_id]->buf))
	{
		// dispatch the next thread in the queue
		// TODO dispatch next thread
	}
}

void * mythread_scheduler(void *context)
{
	// cast context to struct with fp, sp
	// read fp, sp from context
	// save fp, sp to the current thread's tcb

	if(run_queue_count > 0)
	{
		// remove completed threads from the queue
		prune_queue();
		// reprioritize the queue
		prioritize_queue();

		// TODO schedule the new highest priority thread
	}
	else
	{
		alt_printf("Interrupted by the DE2 timer!\n");
	}
	// return struct (to assembly) that is next thread's sp, fp
}

void destroy_thread(tcb *thread)
{
	alt_printf("thread %i destroyed; was scheduled %i times\n", thread->thread_id, thread->scheduled_count);
	free(thread->frame_pointer);
	free(thread);
}

void prune_queue()
{
	int i, j;
	// iterate through queue
	for (i = 0; i < NUM_THREADS; i++)
	{
		tcb *thread = run_queue[i];
		// thread has finished and exited
		if (thread->status == finished)
		{
			// remove the completed thread from the queue
			run_queue[i] = NULL;
			run_queue_count--;
			destroy_thread(thread);
			// shift all remaining threads in the queue up
			for(j = i; j < NUM_THREADS-1; j++)
			{
				run_queue[j] = run_queue[j+1];
			}
			// insert an empty slot at the end of the queue
			run_queue[NUM_THREADS-1] = NULL;
		}
	}
}

void prioritize_queue()
{
	// determine how many open spots are available in the queue
	// snapshot the current highest priority thread
	tcb *current_thread = run_queue[0];
	if (current_thread == NULL)
	{
		alt_printf("run queue is empty");
		return;
	}
	// make the highest priority slot available
	run_queue[0] = NULL;
	int i;
	for(i = 0; i < NUM_THREADS-1; i++)
	{
		// the rest of the queue is empty slots
		if (run_queue[i] == NULL)
		{
			break;
		}
		run_queue[i] = run_queue[i+1];
	}
	// insert the former highest priority thread at the back of the queue
	run_queue[i] = current_thread;
}

void prototype_os()
{
	// initialize the alarm to interrupt after 1 second and set the alarm's callback function
	alt_alarm_start(&alarm, alt_ticks_per_second(), interrupt_handler, NULL);

	int i;
	for (i = 0; i < NUM_THREADS; i++)
	{
		// create new thread; set its function to execute
		tcb *new_thread;
		mythread_create(i, new_thread, mythread, &i);
	}

	if(!setjmp(ret->buf))
	{
		// dispatch the first thread in the run queue
	}

	for (i = 0; i < NUM_THREADS; i++)
	{
		// join the threads
	}

	// loop endlessly
	while(1)
	{
		alt_printf("Hello from uOS!\n");

		// loop; will be interrupted
		for (i = 0; i < 10000; i++);
	}
}

alt_u32 interrupt_handler(void* context)
{
	alt_printf("Interrupted by timer!\n");
	// schedule new thread
	global_flag = 1;
	// reset the alarm to interrupt next in 0.5 seconds
	return ALARMTICKS(5);
}

void reset_global_flag()
{
	global_flag = 0;
}

int get_global_flag()
{
	return global_flag;
}
