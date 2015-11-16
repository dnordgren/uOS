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

typedef struct stack_context {
	int *sp;
	int *fp;
} stack_context;

typedef struct tcb {
	int thread_id;
	int scheduled_count;
	thread_status status;
	int *sp;
	int *fp;
	void (*function)(void *arg);
	void *arg;
} tcb;

// entry point to prototype operating system
void prototype_os();
void mythread(int thread_id);
void mythread_create(int thread_id, tcb *thread, void(*f)(void *arg), void *arg);
void thread_scheduler(void *sp, void *fp);
// callback function for alarm interrupt
alt_u32 interrupt_handler(void* context);
// helper function for pruning run queue of completed threads
void prune_queue();
// helper function for prioritizing threads in run queue
void prioritize_queue();
// helper function used to deallocate tcb for a thread instance
void destroy_thread(tcb *thread);
// helper function used to mark thread as finished
void finish_thread();

// the alarm that will regularly interrupt program execution
alt_alarm alarm;

// the thread to return to once execution of the current thread has finished
tcb *ret;

int global_flag = 0;

tcb *run_queue[NUM_THREADS];
int run_queue_count;
tcb* current_thread;

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
	// allocate memory for the thread's workspace and stack
	tcb *t = (tcb *)malloc(sizeof(tcb) + STACK_SIZE);
	t->thread_id = thread_id;
	// set stack pointer
	t->sp = t + sizeof(tcb);
	// set frame pointer
	t->fp = t->sp + STACK_SIZE;
	// set the thread's function to run
	t->function = f;
	*(t->sp + 18) = finish_thread;

	// add thread to run_queue
	run_queue[thread_id] = t;
	run_queue_count++;
	// thread has been added to queue; set its status to scheduled
	t->status = scheduled;
	thread = t;
}

// TODO
void mythread_join()
{
	// suspend main thread while other threads are running
	// once other threads have died, resume main
	// save a copy of the main thread to return to
	mythread_create(-1, ret, NULL, NULL);
}

void thread_scheduler(void *sp, void *fp)
{
	DISABLE_INTERRUPTS() // TODO why does this not work?
	// save the yielded thread's progress via sp, fp
	current_thread->sp = (int *)sp;
	current_thread->fp = (int *)fp;

	if(run_queue_count > 0)
	{
		// remove completed threads from the queue
		prune_queue();
		// reprioritize the queue
		prioritize_queue();

		// set the next-to-run to run as the current thread
		current_thread = run_queue[0];
		// update next-to-run thread's number of times scheduled count
		current_thread->scheduled_count++;

		// send the next-to-run thread's stack context back to assembly to be run
		sp = current_thread->sp;
		fp = current_thread->fp;

		// TODO start function of new thread
	}
	else
	{
		alt_printf("Interrupted by the DE2 timer!\n");
	}
	ENABLE_INTERRUPTS()
}

void destroy_thread(tcb *thread)
{
	alt_printf("thread %i destroyed; was scheduled %i times\n", thread->thread_id, thread->scheduled_count);
	free(thread);
}

void finish_thread()
{
	current_thread->status = finished;
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
	if (run_queue_count > 0) // TODO do we have to do this?
	{
		global_flag = 1;
	}
	// reset the alarm to interrupt next in 0.5 seconds
	return ALARMTICKS(50);
}

void reset_global_flag()
{
	global_flag = 0;
}

int get_global_flag()
{
	return global_flag;
}

