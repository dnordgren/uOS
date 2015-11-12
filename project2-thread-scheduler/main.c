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
#include "sys/alt_stdio.h"
#include "sys/alt_alarm.h"
#include "alt_types.h"

// compute the number of alarm ticks relative to system ticks per second
#define ALARMTICKS(x) ((alt_ticks_per_second()*(x))/10)
#define NUM_THREADS 12
#define MAX 10000

typedef enum {
	scheduled,
	running,
	finished
} thread_status;

typedef struct tcb {
	int thread_id;
	int run_count;
	thread_status status;
	void* context;
} tcb;

// entry point to prototype operating system
void prototype_os();
void mythread(int thread_id);
void mythread_create(int thread_id, tcb *instance);
void * mythread_scheduler(void *context);
// callback function for alarm interrupt
alt_u32 interrupt_handler(void* context);
// helper function for pruning run queue of completed threads
void prune_queue();
// helper function for prioritizing threads in run queue
void prioritize_queue();

// the alarm that will regularly interrupt program execution
alt_alarm alarm;

tcb *run_queue[NUM_THREADS];
int run_queue_count;

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

void mythread_create(int thread_id, tcb *instance)
{
	tcb *thread = (tcb *)malloc(sizeof(tcb));
	thread->thread_id = thread_id;
	// add thread to run_queue
	run_queue[thread_id] = thread;
	run_queue_count++;
	// thread has been added to queue; set its status to scheduled
	thread->status = scheduled;
	instance = thread;
}

void * mythread_scheduler(void *context)
{
	// do the necessary setup
	if(run_queue_count > 0)
	{
		// suspend the current thread and schedule a new thread

		// remove completed threads from the queue
		prune_queue();
		// reprioritize the queue
		prioritize_queue();

		// schedule the new highest priority thread
	}
	else
	{
		alt_printf("Interrupted by the DE2 timer!\n");
	}
	// do whatever we need to do
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
			// destroy the thread
			free(thread);
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
	int i;
	// initialize the alarm to interrupt after 1 second and set the alarm's callback function
	alt_alarm_start(&alarm, alt_ticks_per_second(), interrupt_handler, NULL);

	// do all the necessary setup
	for (i = 0; i < NUM_THREADS; i++)
	{
		// create the threads
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
	// reset the alarm to interrupt next in 0.5 seconds
	return ALARMTICKS(5);
}
