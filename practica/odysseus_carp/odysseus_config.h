#ifndef ODYSSEUS_CONFIG_H
#define ODYSSEUS_CONFIG_H

#include "Files.h"
#include "IO.h"
#include <fcntl.h>
#include "map.h"


typedef enum{
    MAR,
    ITHACA,
    ESPERANDO,
    EN_PUERTO,
    VIAJANDO,
}Ubicacion;

typedef struct{
    char *name;
    int quantity;
}Food;

typedef struct{
    int id;
    char *objeto;
    char *destino;
    int recompensa;
}Encargo_actual;

typedef struct{
    int fd;
    int ubicacion;
    char *isla_actual;
    Encargo_actual encargo_actual;
}Viaje;

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
    Viaje viaje_actual;
    Mapa mapa;
}Odysseus;


int load_odysseus(const char *path, Odysseus *odysseus);
int addFood(Odysseus *odysseus, const char *food_name, int kg); 
int removeFood(Odysseus *odysseus,char *food_name, int kg);
int removeFoodOfThisType(Odysseus *odysseus, const char *food_name);
int totalFoodWeight(Odysseus *odysseus);
int totalFoodOfThisType(Odysseus *odysseus, const char *food_name);
int eatProvisional(Odysseus *odysseus,int kg);

#endif