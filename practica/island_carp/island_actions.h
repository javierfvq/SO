#ifndef ISLAND_ACTIONS_H
#define ISLAND_ACTIONS_H

#include "island_config.h"
#include "queue.h"
#include "trama.h"

enum EstadoPuerto {
    NO_SHIP,
    DOCKED, 
    WAITING
};

typedef struct {
    char *names[FD_SETSIZE];
    int status[FD_SETSIZE];
    int count_docked;
    QueuePort queue;
} Port;

void detectTrama (int fd, Trama *trama, Island_CONFIG *island, Port *port);
void abandonaPuerto (int fd, Port *port);

#endif