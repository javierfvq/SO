#ifndef ISLAND_CONFIG_H
#define ISLAND_CONFIG_H

#include "Files.h"
#include "IO.h"
#include "sphragis.h"
#include <fcntl.h>
#include "Connection.h"
#include "trama.h"

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
    char *stock_path;
    char *folder;
    char *island_ip;
    int  island_port;
    int max_capacity;
    int counter;
    Route *routes;
    int product_count;
    Stock *products;
}Island_CONFIG;

int load_config_island(const char *path, Island_CONFIG *island);
int load_Binary(const char *path,Island_CONFIG *island);
int check_island_routes(Island_CONFIG *island);

#endif