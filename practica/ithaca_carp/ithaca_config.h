#ifndef ITHACA_CONFIG_H
#define ITHACA_CONFIG_H


#include "Files.h"
#include "IO.h"
#include "trama.h"
#include <fcntl.h>

enum estadoViaje {
    AVAILABLE,
    ASSIGNED,
    FAILED
};

typedef struct {
    char *object;
    char *file;
    char *destination;
    int reward;
    int id;
    int status;
    char *assigned; // a quien se le ha asignado el viaje
} Voyage;

typedef struct {
    char *serverName;
    char *objectsFolder;
    char* ip;
    int port;
    int num_voyages;
    Voyage *voyages;
} Ithaca;

int loadIthaca(const char *path, Ithaca *ithaca);
int loadVoyages(const char *path, Ithaca *ithaca);


#endif