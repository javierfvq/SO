#ifndef ODYSSEUS_H      
#define ODYSSEUS_H     
#include "Files.h"


typedef struct{
    char *name;
    int quantity;
}Food;

typedef struct{
    char *name;
    char *folder;
    char *itaca_ip;
    int  itaca_port;
    char *ip_start;
    int  port_start;
    int money;
    int food_count;
    Food *food;
}Odysseus;

int load_odysseus(const char *path, Odysseus *odysseus);

#endif