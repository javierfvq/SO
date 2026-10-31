#define _GNU_SOURCE
#include "ithaca_config.h"

int  loadIthaca (const char *filepath, Ithaca *ithaca) {

    int fd = open(filepath, O_RDONLY);

    if (fd < 0) {
        printError("Error opening Ithaca file\n");
        return -1 ;
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
            printError("Error is missing a field\n");
            close(fd);
            return -1;
        }

        ithaca -> serverName = serverName;
        ithaca -> objectsFolder = objectsFolder;
        ithaca -> ip = ip;
        ithaca -> port = atoi(port);
        free(port);
        close(fd);
        return 0;
    }

}

int loadVoyages (const char* filepath, Ithaca *ithaca) {

    int fd = open(filepath, O_RDONLY);

    if (fd < 0) {
        printError("Error opening voyages file\n");
        return -1;
    } else {
        
        ithaca -> num_voyages = 0;
        ithaca -> voyages = NULL;
        char *object, *file, *destination, *reward;

        while ((object = parsear(fd, ' ')) != NULL) {
            if (object[0] == '\0') {
                free(object);
                continue;
            }
            file = parsear(fd, ' ');
            destination = parsear(fd, ' ');
            reward = parsear(fd, ' ');

            if (object == NULL || file == NULL || destination == NULL || reward == NULL) {
                free(object);
                free(file);
                free(destination);
                free(reward);
                printError("Error is missing a field\n");
                close(fd); 
                return -1 ;
            } else {
               Voyage *tmp= realloc(ithaca->voyages, sizeof(Voyage) * (ithaca->num_voyages + 1));
               if (tmp == NULL) {
                   printError("Error allocating memory for voyages\n");
                   free(object);
                   free(file);
                   free(destination);
                   free(reward);
                   close(fd);
                   return -1;
               }
               ithaca->voyages = tmp;
               ithaca->voyages[ithaca->num_voyages].object = object;
               ithaca->voyages[ithaca->num_voyages].file = file;
               ithaca->voyages[ithaca->num_voyages].destination = destination;
               ithaca->voyages[ithaca->num_voyages].reward = atoi(reward);
               ithaca->voyages[ithaca->num_voyages].id = ithaca->num_voyages + 1;
               ithaca->voyages[ithaca->num_voyages].assigned = NULL;
               ithaca->voyages[ithaca->num_voyages].status = AVAILABLE;
               free(reward);
               
               ithaca->num_voyages++;

            }

        }
        close(fd);
        return 0; 
    }
}


