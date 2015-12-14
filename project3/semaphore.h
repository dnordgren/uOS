/*
 * semaphore.h
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "thread_handler.h"

/**
 * Implements the nodes of a linked list for a semaphore wait queue where each
 * node's value is a tcb, representing a thread.
 */
typedef struct wait_q {
	tcb *thread;
	struct wait_q *next;
} wait_q;

/**
 * Represents a semaphore
 */
typedef struct sem {
	int value;
	int wait_count;
	wait_q *head; /* The first element of the semaphore's wait queue (a linked list) */
	wait_q *tail; /* The last element of the semaphore's wait queue (a linked list) */
} sem;

/**
 * Allocates a new semaphore with the specified value.
 */
sem * sem_create(int value);
/**
 * Increment the value of a semaphore. If up on semaphore with a value of zero,
 * unblock the first thread in the semaphore's wait queue.
 */
void sem_up(sem *sem);
/**
 * Decrement the value of a semaphore. If down on a semaphore with a value of zero,
 * block the requesting thread and add to the semaphore's wait queue.
 */
void sem_down(sem *sem);
/**
 * Deallocates a semaphore.
 */
void sem_delete(sem *sem);
/**
 * Get a semaphore's wait count.
 */
int sem_wait_count(sem *sem);
/**
 * Get a semaphore's value.
 */
int sem_value(sem *sem);

#endif /* SEMAPHORE_H_ */
