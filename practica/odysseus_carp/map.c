#include "map.h"


void initMapa(Mapa *mapa){
    mapa->islands = NULL;
    mapa->count = 0;
}
IslandVisited *getIsland(Mapa *mapa, const char *island_name){
    for (int i = 0; i < mapa->count; i++) {
        if (strcmp(mapa->islands[i].island_name, island_name) == 0) {
            return &mapa->islands[i];
        }
    }
    return NULL;
}
int canIgoToIsland(Mapa *mapa, const char *island_name,char *current_island){
    IslandVisited *current = getIsland(mapa, current_island);
    if (current == NULL) {
        return MAP_ERROR;
    }
    for (int i = 0; i < current->connection_count; i++) {
        if (strcmp(current->connections[i], island_name) == 0) {
            return MAP_OK;
        }
    }
    return MAP_NO_CONNECTION;
}

int addIsland(Mapa *mapa, const char *island_name, const char *island_ip, int island_port,char *current_island){
    IslandVisited *newIsland;
    if (getIsland(mapa, island_name) != NULL) {
        return addConnection(mapa, island_name, current_island);
    }
    IslandVisited *temp = realloc(mapa->islands, (mapa->count + 1) * sizeof(IslandVisited));
    if (temp == NULL) {
        return MAP_ERROR;
    }
    mapa->islands = temp;
    newIsland = &mapa->islands[mapa->count];
    newIsland->island_name = malloc(strlen(island_name) + 1);
    if (newIsland->island_name == NULL) {
        return MAP_ERROR;
    }
    strcpy(newIsland->island_name, island_name);
    
    newIsland->island_ip = malloc(strlen(island_ip) + 1);
    if (newIsland->island_ip == NULL) {
        free(newIsland->island_name);
        return MAP_ERROR;
    }
    strcpy(newIsland->island_ip, island_ip);
    newIsland->island_port = island_port;
    newIsland->connections = NULL;
    newIsland->connection_count = 0;
    mapa->count++;
    addConnection(mapa, island_name, current_island);
    return MAP_OK;
}
static int addOneConnection(IslandVisited *island, const char *connection_name) {
    if (strcmp(connection_name, island->island_name) == 0) {
    return MAP_OK;
    }
    for(int i = 0; i < island->connection_count; i++) {
        if (strcmp(island->connections[i], connection_name) == 0) {
            return MAP_OK;
        }
    }
    char **temp = realloc(island->connections, (island->connection_count + 1) * sizeof(char *));
    if (temp == NULL) {
        return MAP_ERROR;
    }
    island->connections = temp;
    island->connections[island->connection_count] = malloc(strlen(connection_name) + 1);
    if (island->connections[island->connection_count] == NULL) {
        return MAP_ERROR;   
    }
    strcpy(island->connections[island->connection_count], connection_name);
    island->connection_count++;
    return MAP_OK; 
}
int addConnection(Mapa *mapa, const char *island_name,char *current_island){
    IslandVisited *island1, *island2;
    if (island_name == NULL || current_island == NULL) {
        return MAP_OK;
    }
    island1=getIsland(mapa, island_name);
    island2=getIsland(mapa, current_island);
    if (island1 == NULL || island2 == NULL) {
        return MAP_ERROR;   
    }
    if (addOneConnection(island1, current_island) != MAP_OK) {
        return MAP_ERROR;
    }
    if (addOneConnection(island2, island_name) != MAP_OK) {
        return MAP_ERROR;
    }
    return MAP_OK;
}
void freeMapa(Mapa *mapa){
    for(int i = 0; i < mapa->count; i++) {
        free(mapa->islands[i].island_name);
        free(mapa->islands[i].island_ip);
        for(int j = 0; j < mapa->islands[i].connection_count; j++) {
            free(mapa->islands[i].connections[j]);
        }
        free(mapa->islands[i].connections);
        mapa->islands[i].connections = NULL;
        mapa->islands[i].connection_count = 0;
    }
    free(mapa->islands);
    mapa->islands = NULL;
    mapa->count = 0;
}

