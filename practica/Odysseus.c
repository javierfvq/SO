#include "Odysseus.h"
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




void load_odysseus(const char *path, Odysseus *odysseus) {
   int fd=open(path,O_RDONLY);

   if (fd<0){
      printError("El path que has introducido es incorrecto\n");
      return ;
   }else{
         
         char *folder = parsear(fd, ' ');
         char *name = parsear(fd, ' ');
         char *itaca_ip = parsear(fd, ' ');
         char *itaca_port = parsear(fd, ' ');
         char *ip_start = parsear(fd, ' ');
         char *port_start = parsear(fd, ' ');
         char *money = parsear(fd, ' ');
         char *food_count = parsear(fd, '\n');
         
        if (folder == NULL|| itaca_ip == NULL || name == NULL || itaca_port == NULL || ip_start == NULL || port_start == NULL || money == NULL || food_count == NULL) {
            free(folder);
            free(itaca_ip);
            free(name);
            free(itaca_port);
            free(ip_start);
            free(port_start);
            free(money);
            free(food_count);
            printError("Error falta algun campo\n");
            close(fd)
            return;
        }else{
            
            odysseus->folder = folder;
            odysseus->name = name;
            odysseus->itaca_ip = itaca_ip;
            odysseus->itaca_port = atoi(itaca_port);
            free(itaca_port);
            odysseus->ip_start = ip_start;
            odysseus->port_start = atoi(port_start);
            free(port_start);
            odysseus->money = atoi(money);
            free(money);
            odysseus->food_count = atoi(food_count);
            free(food_count);
            
            odysseus->food = malloc(odysseus->food_count * sizeof(Food));


            
            for (int i = 0; i < odysseus->food_count; i++) {
               char *food_name = parsear(fd, ' ');
               char *food_quantity = parsear(fd, '\n');
                if (food_name == NULL || food_quantity == NULL) {
                  printError("Error falta algun campo\n");
                  return;
                }else{
                  odysseus->food[i].name = food_name;
                  odysseus->food[i].quantity = atoi(food_quantity);
                  free(food_quantity);
                }
            }
        }
   }
   close(fd);
}