#ifndef ITHACA_H
#define ITHACA_H

typedef struct {
    char *object;
    char *file;
    char *destination;
    int reward;
    int id;
} Voyage;

typedef struct {
    char *serverName;
    char *objectsFolder;
    char* ip;
    int port;
    int num_voyages;
    Voyage *voyages;
} Ithaca;


int loadIthaca (char *filepath, Ithaca *ithaca);
int  loadVoyages (char* filepath, Ithaca *ithaca);


#endif 