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


void load_Binary(const char *path,Island_CONFIG *island) {
  Stock temp;
  int leido;
  
  
  int fd=open(path, O_RDONLY);  
  if(fd == -1){
    printError("Error al abrir el archivo binario\n");
    return;
  }else{
    island->products=NULL;
    island->product_count=0;
    
    while((leido=(int)read(fd, &temp, sizeof(Stock))) >0){
      island->products=realloc(island->products, (island->product_count + 1) * sizeof(Stock));
      island->products[island->product_count]=temp;
       island->product_count++;
    }
    if (leido < 0) {
    printError("Error leyendo el fichero de stock\n");
    }
    close(fd);
  }

}


void check_island_routes(Island_CONFIG *island){
    SPHRAGIS_Island sphragis_island;
    char **names= malloc(island->counter * sizeof(char*));

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
        return;
    }

    Route *valid_routes= NULL;
    valid_routes = malloc(valid_count * sizeof(Route));
   

    int j=0;
    for(int i=0; i<island->counter; i++){
        if (names[i] != NULL){
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
}


void load_config_island(const char *path, Island_CONFIG *island) {
   int fd=open(path,O_RDONLY);
   
   if (fd<0){
      printError("El path que has introducido es incorrecto\n");
      return ;
   }else{
         char *name = parsear(fd, ' ');
         char *folder = parsear(fd, ' ');
         char *island_ip = parsear(fd, ' ');
         char *island_port = parsear(fd, ' ');
         char *max_capacity = parsear(fd, ' ');
         
        if( name == NULL || folder == NULL || island_ip == NULL || island_port == NULL || max_capacity == NULL) {
            printError("Error falta algun campo\n");
            close(fd);
            return;
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
                    close(fd);
                    return;
                }else{
                    island->routes = realloc(island->routes, (island->counter + 1) * sizeof(Route));
                    island->routes[island->counter].name = route_name;
                    island->routes[island->counter].ip = route_ip;
                    island->routes[island->counter].port = atoi(route_port);
                    free(route_port);
                    island->counter++;
                }

            }
          

           check_island_routes(island); 
        }
   }
   close(fd);
}