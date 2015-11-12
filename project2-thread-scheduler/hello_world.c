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
#define NUMTHREADS 12

// entry point to prototype operating system
void prototype_os();

void mythread(int thread_id);

void * mythread_scheduler(void *context);

// callback function for alarm interrupt
alt_u32 interrupt_handler(void* context);

// the alarm that will regularly interrupt program execution
alt_alarm alarm;

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
		printf("This is message %d of thread # %d.\n", i, thread_id);
		for (j = 0; j < MAX; j++);
	}
}

void prototype_os()
{
	int i;
	// initialize the alarm to interrupt after 1 second and set the alarm's callback function
	alt_alarm_start(&alarm, alt_ticks_per_second(), interrupt_handler, NULL);

	// do all the necessary setup
	for (i = 0; i < NUMTHREADS; i++)
	{
		// create the threads
	}

	for (i = 0; i < NUMTHREADS; i++)
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
	global_flag = 1;
	alt_printf("Interrupted by timer!\n");
	// reset the alarm to interrupt next in 0.5 seconds
	return ALARMTICKS(5);
}
