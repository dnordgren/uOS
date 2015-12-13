/*
 * semaphore.h
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "thread_handler.h"

typedef struct wait_q {
	tcb *thread;
	struct wait_q *next;
} wait_q;

typedef struct sem {
	int value;
	int wait_count;
	wait_q *head;
	wait_q *tail;
} sem;

sem * sem_create(int value);
void sem_up(sem *sem);
void sem_down(sem *sem);
void sem_delete(sem *sem);
int sem_wait_count(sem *sem);
int sem_value(sem *sem);

#endif /* SEMAPHORE_H_ */
