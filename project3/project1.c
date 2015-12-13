#include <stdio.h>

#include "alarm_handler.h"
#include "thread_handler.h"
#include "semaphore.h"

/* a delay time used to adjust the frequency of print messages */
#define MAX 100000

#define NUM_BEES 10 /* the number of bees */
#define HONEY_POT_SIZE 30 /* the capacity of the honey pot */
#define BEAR_THREAD_LIFE 5 /* the number of times the bear will eat honey before dying */
#define BEE_THREAD_LIFE 15 /* the number of times a bee will collect honey before dying */

int num_honey = 0; /* pot starts empty */
int bear_eat_count = 0; /* how many times the bear has eaten honey */
int bee_collect_count = 0; /* how many times a bee has deposited honey */

sem *pot_full;
sem *mutex;

void bear_thangs()
{
	while (bear_eat_count < BEAR_THREAD_LIFE)
	{
		sem_down(pot_full);
		num_honey = 0;
		bear_eat_count++;
		printf("I'm a bear and I just ate honey for the %ith time. Mmmmmmmmmmmmm, honey.\n", bear_eat_count);
		sem_up(mutex);
	}
}

void bee_stuff()
{
	while (bee_collect_count < NUM_BEES*BEE_THREAD_LIFE)
	{
		sem_down(mutex);
		num_honey++;
		bee_collect_count++;
		printf("Buzz buzz I'm a bee, I just collected honey buzz buzz. The pot has %i honey.\n", num_honey);
		if (num_honey == HONEY_POT_SIZE)
		{
			sem_up(pot_full); /* the bee that fills the pot wakes the bear */
		}
		else
		{
			sem_up(mutex);
		}
	}
}
    
void os_primitive()
{
	/* initialize semaphores */
	pot_full = sem_create(0); /* honey pot not full to start */
	mutex    = sem_create(1); /* mutual exclusion for honey pot access */

    unsigned int i;
    tcb *thread;

    for (i = 0; i < NUM_BEES; i++)
    {
    	thread = mythread_create(i, 4096, bee_stuff);
    	mythread_start(thread);
    	mythread_join(thread);
    }

    thread = mythread_create(10, 4096, bear_thangs);
    mythread_start(thread);
    mythread_join(thread);
    
    if ( start_alarm_succeed() )
        printf ("Started the alarm successfully\n");
    else
        printf ("Unable to start the alarm\n");

    /* an endless while loop */
    while (1)
    {
        printf ("This is the OS primitive for my exciting CSE351 course projects!\n");
        
        /* delay print for a while */
        for (i = 0; i < 10*MAX; i++);
    }
}

int main()
{
    os_primitive();
    return 0;
}
