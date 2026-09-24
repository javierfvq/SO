#ifndef ISLAND_H 
#define ISLAND_H     
#include "Files.h"




typedef struct{
    char name[100];
    int amount;
    int price;
}Stock;

typedef struct{
    char *name;
    char *ip;
    int port;
}Route;

typedef struct{
    char *name;
    char *folder;
    char *island_ip;
    int  island_port;
    int max_capacity;
    int counter;
    Route *routes;
    int product_count;
    Stock *products;

}Island_CONFIG;

int  load_config_island(const char *path, Island_CONFIG *odysseus);
#endif