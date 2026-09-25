#define _GNU_SOURCE

#include "Island.h"
#include "IO.h"
#include <stdio.h>
#include "Files.h" 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "sphragis.h"



static Island_CONFIG island;





void free_island(void){

    free(island.name);
    island.name = NULL;

    free(island.folder);
    island.folder = NULL;
    free(island.island_ip);
    island.island_ip = NULL;

    for(int i=0; i<island.counter; i++){
        free(island.routes[i].name);
        island.routes[i].name = NULL;
        free(island.routes[i].ip);
        island.routes[i].ip = NULL;
    }
    free(island.routes);
    island.routes = NULL;
    free(island.products);
    island.products = NULL;
    island.product_count=0;
    island.counter=0;

}

void signal_handler(int signum) {
    char *buffer;
    if (signum == SIGINT) {
        if (asprintf(&buffer, "\n%s closes its port.\n", island.name) == -1) {
            return;
        }
        printMessage(buffer);
        free(buffer);
        free_island();
        exit(0);
    }

}




int load_Binary(const char *path,Island_CONFIG *island) {
  Stock temp;
  int leido;
  
  
  int fd=open(path, O_RDONLY);  
  if(fd == -1){
    printError("Error al abrir el archivo binario\n");
    return -1 ;
  }else{
    island->products=NULL;
    island->product_count=0;
    
    while((leido=(int)read(fd, &temp, sizeof(Stock))) >0){
     if (leido != sizeof(Stock)) {
            printError("Error reading the binary file\n");
            close(fd);
            return -1;
        }
      island->products=realloc(island->products, (island->product_count + 1) * sizeof(Stock));
      island->products[island->product_count]=temp;
       island->product_count++;
    }
    if (leido < 0) {
    printError("Error leyendo el fichero de stock\n");
    close(fd);
    return -1;
    }
    close(fd);
    return 0;
   
  }

}


int  check_island_routes(Island_CONFIG *island){
    SPHRAGIS_Island sphragis_island;
    char **names=NULL;

    if (island->counter > 0) {
        names = malloc(island->counter * sizeof(char*));
        if (names == NULL) {
            printError("Error al asignar memoria para las rutas\n");
            return -1;
        }
    }
    for(int i=0; i<island->counter; i++){
            names[i]=island->routes[i].name;
     }
          
    sphragis_island.name = island->name;
    sphragis_island.known_islands = names;
    sphragis_island.known_island_count = island->counter;
    int valid_count = SPHRAGIS_filter_island_configuration(&sphragis_island);

    if (valid_count < 0) {
        printError("Error encontrado en las rutas\n");
        for(int i = 0; i < island->counter; i++) {
            free(island->routes[i].name);
            free(island->routes[i].ip);
        }
        free(island->routes);
        free(names);
        island->routes = NULL;
        island->counter = 0;
        return -1;
    }

    Route *valid_routes= NULL;
    if (valid_count > 0) {
        valid_routes= malloc(valid_count * sizeof(Route));
        if (valid_routes == NULL) {
            printError("Error to allocate memory for valid routes\n");
            free(names);
            return -1;
        }
    }
   
   

    int j=0;
    for(int i=0; i<island->counter; i++){
        int valida=0;
        for (int k=0; k<valid_count; k++){
            if ((island->routes[i].name==names[k])){//Lo hago para no acceder al contenido de un posible contenido liberado,de esta manera solo miro si la direccion de memoria es la misma ,o si ya se ha liberado la memoria y no quiero acceder a ella
                valida=1;
                break;
            }
        }
        if (valida==1){
            valid_routes[j].name = island->routes[i].name;
            valid_routes[j].ip = island->routes[i].ip;
            valid_routes[j].port = island->routes[i].port;
            j++;
        }else{
            free(island->routes[i].ip);
        }
    }
   
    free(names);
    free(island->routes);
    island->routes = valid_routes;
    island->counter = valid_count;
    return 0;
}


int  load_config_island(const char *path, Island_CONFIG *island) {
   int fd=open(path,O_RDONLY);
   
   if (fd<0){
      printError("El path que has introducido es incorrecto\n");
      return -1;
   }else{
         char *name = parsear(fd, ' ');
         char *folder = parsear(fd, ' ');
         char *island_ip = parsear(fd, ' ');
         char *island_port = parsear(fd, ' ');
         char *max_capacity = parsear(fd, ' ');
         
        if( name == NULL || folder == NULL || island_ip == NULL || island_port == NULL || max_capacity == NULL) {
            printError("Error falta algun campo\n");
            free(name);
            island->name = NULL;
            free(folder);
            island->folder = NULL;
            free(island_ip);
            island->island_ip = NULL;
            free(island_port);
            island->island_port = 0;
            free(max_capacity);
            island->max_capacity = 0;

            close(fd);
            return -1;
        }else{
            island->name = name;
            island->folder = folder;
            island->island_ip = island_ip;
            island->island_port = atoi(island_port);
            free(island_port);
            island->max_capacity = atoi(max_capacity);
            free(max_capacity);
            char *trash= parsear(fd, '\n');
            free(trash);
            char *route_name;
            island->counter = 0;
            island->routes = NULL;
            
            while((route_name = parsear(fd, ' '))!= NULL) {
                char *route_ip = parsear(fd, ' ');
                char *route_port = parsear(fd, '\n');
                if(route_ip == NULL || route_port == NULL){
                    printError("Error falta algun campo en las rutas\n");
                    free(route_name);
                    free(route_ip);
                    free(route_port);
                    for(int i = 0; i < island->counter; i++) {
                        free(island->routes[i].name);
                        free(island->routes[i].ip);
                    }
                    free(island->routes);
                    island->routes = NULL;
                    island->counter = 0;
                    close(fd);
                    return -1;
                }else{
                    island->routes = realloc(island->routes, (island->counter + 1) * sizeof(Route));
                    island->routes[island->counter].name = route_name;
                    island->routes[island->counter].ip = route_ip;
                    island->routes[island->counter].port = atoi(route_port);
                    free(route_port);
                    island->counter++;
                }

            }
            close(fd);
            if (check_island_routes(island) == -1) {
                return -1;
            }

           
        }
   }
    return 0;

}

void printMessageInitialization(Island_CONFIG *island) {
    char *buffer;
    if (asprintf(&buffer, "Island %s initialized.\n", island->name) == -1) {
        return;
    }
    printMessage(buffer);
    free(buffer);
    if (island->max_capacity== 1) {
        if (asprintf(&buffer, "Port capacity: %d ship.\n", island->max_capacity) == -1) {
            return;
        }
        printMessage(buffer);
        free(buffer);
    } else if (island->max_capacity > 1) {
        if (asprintf(&buffer, "Port capacity: %d ships.\n", island->max_capacity) == -1) {
            return;
        }
        printMessage(buffer);
        free(buffer);
    }
  
    if (asprintf(&buffer, "%d sea routes loaded.\n", island->counter) == -1) {
        return;
    }
    printMessage(buffer);
    free(buffer);

    if (asprintf(&buffer, "%d products available.\n", island->product_count) == -1) {
        return;
    }
    printMessage(buffer);
    free(buffer);
 
}

int main(int argc,char *argv[]){
   

    signal(SIGINT,signal_handler);

    if (argc!=3){
        printError("Error,usage: ./island <config_file> <stock_file>\n");
        return 1;
    }else{
        if (load_config_island(argv[1],&island) == -1) {
            free_island();
            printError("Error to load island configuration\n");
            return 1;
        }
        if (load_Binary(argv[2],&island) == -1) {
            free_island();
            printError("Error to load stock binary file\n");
            return 1;
        }

        printMessageInitialization(&island);

        while(1){
            pause();
        }
        


    }

}