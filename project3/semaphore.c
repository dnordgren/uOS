/*
 * semaphore.c
 */

#include "semaphore.h"
#include "thread_handler.h"

#include <stdlib.h> /* NULL */
#include <stdio.h> /* printf */

#define DISABLE_INTERRUPTS() {  \
    asm("wrctl status, zero");  \
}

#define ENABLE_INTERRUPTS() {   \
    asm("movi et, 1");          \
    asm("wrctl status, et");    \
}

sem * sem_create(int value)
{
	sem *semaphore = (sem *)malloc(sizeof(sem));
	semaphore->value = value;
	semaphore->wait_count = 0;
	semaphore->head = NULL;
	semaphore->tail = NULL;
	return semaphore;
}

void sem_up(sem *sem)
{
	DISABLE_INTERRUPTS()
	sem->value++;
	if (sem->head != NULL)
	{
		printf("unblocking thread %u\n", sem->head->thread->tid);
		mythread_start(sem->head->thread); /* unblock the longest waiting thread*/
		sem->head = sem->head->next;
		free(sem->head); /* remove the thread from the semaphore's queue */
	}
	ENABLE_INTERRUPTS()
}

void sem_down(sem *sem)
{
	DISABLE_INTERRUPTS()
	if (sem->value > 0)
	{
		sem->value--; /* resource(s) available for requesting thread */
		ENABLE_INTERRUPTS()
        return;
	}
	else /* down on zero; sleep the current thread */
	{
		wait_q *queue_item = (wait_q *)malloc(sizeof(wait_q)); /* create new queue item */
		printf("Adding thread %u to semaphore wait queue\n", current_running_thread->tid);
		queue_item->thread = current_running_thread;

		if (sem->head == NULL) /* queue not initialized */
		{
			sem->head = queue_item;
			sem->tail = queue_item;
			queue_item->next = NULL;
		}
		else /* queue the new thread */
		{
			sem->tail->next = queue_item;
			sem->tail = queue_item;
			queue_item->next = NULL;
		}
		sem->wait_count++;
		mythread_block(queue_item->thread);
		ENABLE_INTERRUPTS()
		while(sem->value == 0); /* wait until unblocked */
		DISABLE_INTERRUPTS()
        /* (atomic) the resource will be used by the newly unblocked thread next */
        /* time its scheduled so remove a resource now to prevent another thread */
        /* from reserving a resource that's unavailable */
		sem->value--;
		ENABLE_INTERRUPTS()
        return;
	}
}

void sem_delete(sem *sem)
{
	printf("deleted a semaphore...\n");
	if (sem->head != NULL)
	{
		wait_q *current_thread = sem->head;
		while (current_thread != NULL)
		{
			wait_q *next_thread = current_thread->next;
			free(current_thread);
			current_thread = next_thread;
		}
	}
	free(sem->head);
	free(sem);
}

int sem_wait_count(sem *sem)
{
	return sem->wait_count;
}

int sem_value(sem *sem)
{
	return sem->value;
}
