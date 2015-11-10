/*
 * Project 1 -- main.c
 * @author Rees Klintworth
 * @author Derek Nordgren
 *
 * This program runs a simple loop endlessly. The loop is periodically interrupted by a system alarm,
 * configured to interrupt the normal program execution at regular intervals.
 */

#include <stdlib.h>
#include "sys/alt_stdio.h"
#include "sys/alt_alarm.h"
#include "alt_types.h"

// compute the number of alarm ticks relative to system ticks per second
#define ALARMTICKS(x) ((alt_ticks_per_second()*(x))/10)

// entry point to prototype operating system
void uOS();

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

void uOS()
{
	// initialize the alarm to interrupt after 1 second and set the alarm's callback function
	alt_alarm_start(&alarm, alt_ticks_per_second(), interrupt_handler, NULL);

	// loop endlessly
	while(1)
	{
		alt_printf("Hello from uOS!\n");

		// loop; will be interrupted
		int j;
		for (j = 0; j < 10000; j++);
		//asm("nop");
	}
}

alt_u32 interrupt_handler(void* context)
{
	alt_printf("Interrupted by timer!\n");
	// reset the alarm to interrupt next in 0.5 seconds
	return ALARMTICKS(5);
}
