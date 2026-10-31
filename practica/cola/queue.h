#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>

#define MAX_COLA_SIZE 15 
#define QUEUE_OK 0
#define QUEUE_ERROR_FULL -1
#define QUEUE_ERROR_EMPTY -2


typedef struct{
    int fds[MAX_COLA_SIZE];
    int count;
}QueuePort;


void    QUEUE_init (QueuePort * queue);
int     QUEUE_isEmpty (QueuePort *queue);
int     QUEUE_add(QueuePort * queue, int fd);
int     QUEUE_get(QueuePort * queue);
int    QUEUE_remove(QueuePort * queue,int fd);


#endif