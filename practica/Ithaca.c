#include "IO.h"
#include "Files.h" 
#include "Ithaca.h" 


#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

void loadIthaca (char *filepath, Ithaca *ithaca) {

    int fd = open(filepath, O_RDONLY);

    if (fd < 0) {
        printError("El path que has introducido es incorrecto\n");
        return;
    } else {

        char *serverName = parsear(fd, ' ');
        char *objectsFolder = parsear(fd,' ');
        char *ip = parsear(fd, ' ');
        char *port = parsear(fd, ' ');

        if (ip == NULL || objectsFolder == NULL || serverName == NULL || port == NULL) {
            free(serverName);
            free(objectsFolder);
            free(ip);
            free(port);
            printError("Error falta algun campo\n");
            close(fd);
            return;
        }

        ithaca -> serverName = serverName;
        ithaca -> objectsFolder = objectsFolder;
        ithaca -> ip = ip;
        ithaca -> port = atoi(port);
        free(port);
    

        close(fd);
    }

}


void loadVoyages (char* filepath, Ithaca *ithaca) {

    int fd = open(filepath, O_RDONLY);

    if (fd < 0) {
        printError("El path que has introducido es incorrecto\n");
        return;
    } else {
        
        ithaca -> num_voyages = 0;
        ithaca -> voyages = NULL;
        char *object, *file, *destination, *reward;

        while ((object = parsear(fd, ' ')) != NULL) {
            file = parsear(fd, ' ');
            destination = parsear(fd, ' ');
            reward = parsear(fd, ' ');

            if (object == NULL || file == NULL || destination == NULL || reward == NULL) {
                free(object);
                free(file);
                free(destination);
                free(reward);
                printError("Error falta algun campo\n");
                close(fd);
                return;
            } else {
                
               ithaca->voyages = realloc(ithaca->voyages, sizeof(Voyage) * (ithaca->num_voyages + 1));
               
               ithaca->voyages[ithaca->num_voyages].object = object;
               ithaca->voyages[ithaca->num_voyages].file = file;
               ithaca->voyages[ithaca->num_voyages].destination = destination;
               ithaca->voyages[ithaca->num_voyages].reward = atoi(reward);
               free(reward);
               
               ithaca->num_voyages++;

            }

        }

        close(fd);
    }
}