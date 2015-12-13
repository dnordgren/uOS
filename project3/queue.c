#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include "queue.h"
#include "thread_handler.h"

static Q_type queue = {NULL, NULL, 0};

void enqueue(void *data)
{
    E_type  *elem;
    
    if ((elem = (E_type *)malloc(sizeof(E_type))) == NULL)
    {
        printf("Unable to allocate space!\n");
        exit(1);
    }
    elem->data = data;
    elem->next = NULL;
    
    if (queue.head == NULL)
        queue.head = elem;
    else
        queue.tail->next = elem;
    queue.tail = elem;

    queue.size++;
}

void *dequeue()
{
    E_type  *elem;
    void    *data = NULL;
    
    if (queue.size != 0)
    {
        elem = queue.head;
        tcb *thread = (tcb *)elem->data;
        while ((thread != NULL) && (thread->state == BLOCKED))
        {
        	elem = elem == queue.tail ? queue.head : elem->next;
        	thread = (tcb *)elem->data;
        }

        if (queue.size == 1)
            queue.tail = NULL;
        
        if (elem == queue.head)
        {
            queue.head = elem->next;
        }
        else
        {
        	E_type *queue_item = queue.head;
        	while (queue_item->next != elem)
        	{
        		queue_item = queue_item->next;
        	}

        	queue_item->next = elem->next;

        	if (queue.tail == elem)
        	{
        		queue.tail = queue_item;
        	}
        }

        queue.size--;
        data = elem->data;
        free(elem);
    }
        
    return data;
}

unsigned int getQsize()
{
    return queue.size;
}
