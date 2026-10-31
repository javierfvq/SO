#include "queue.h"

void QUEUE_init(QueuePort *queue) {
    queue->count = 0; 
    for (int i = 0; i < MAX_COLA_SIZE; i++) {
    queue->fds[i] = -1;
    }
}
int QUEUE_isEmpty(QueuePort *queue) {
    return queue->count == 0;
}
int QUEUE_add(QueuePort *queue, int fd){
    if (queue->count >= MAX_COLA_SIZE) {
        return QUEUE_ERROR_FULL; 
    }
    queue->fds[queue->count] = fd;
    queue->count++;
    return QUEUE_OK; 
}
int QUEUE_get(QueuePort *queue) {
    if (QUEUE_isEmpty(queue)) {
        return QUEUE_ERROR_EMPTY; 
    }
    int fd = queue->fds[0];
    for (int i = 1; i < queue->count; i++) {
        queue->fds[i - 1] = queue->fds[i];
    }
    queue->count--;
    return fd;
}
int QUEUE_remove(QueuePort *queue,int fd){
    int found = 0;
    for (int i = 0; i < queue->count; i++) {
        if (queue->fds[i] == fd) {
            found = 1;
        }
        if (found && i < queue->count - 1) {
            queue->fds[i] = queue->fds[i + 1];
        }
    }
    if (found) {
        queue->count--;
        return QUEUE_OK; 
    } else {
        return QUEUE_ERROR_EMPTY; 
    }
}
