#ifndef MAP_H
#define MAP_H


#include <stdlib.h>
#include <string.h>

#define MAP_OK 0
#define MAP_ERROR -1
#define MAP_NO_CONNECTION -2


typedef struct{
    char *island_name;
    char *island_ip;
    int island_port;
    char **connections;
    int connection_count;
}IslandVisited;


typedef struct{
    IslandVisited *islands;
    int count;
}Mapa;


void initMapa(Mapa *mapa);
IslandVisited *getIsland(Mapa *mapa, const char *island_name);
int canIgoToIsland(Mapa *mapa, const char *island_name,char *current_island);
int addIsland(Mapa *mapa, const char *island_name, const char *island_ip, int island_port,char *current_island);
int addConnection(Mapa *mapa, const char *island_name,char *current_island);
void freeMapa(Mapa *mapa);








#endif 