#define _GNU_SOURCE

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


static Ithaca ithaca;




void freeIhaca(void){
    free(ithaca.serverName);
    ithaca.serverName = NULL;
    free(ithaca.objectsFolder);
    ithaca.objectsFolder = NULL;
    free(ithaca.ip);
    ithaca.ip = NULL;
    ithaca.port = 0;
    for (int i = 0; i < ithaca.num_voyages; i++) {
        free(ithaca.voyages[i].object);
        ithaca.voyages[i].object = NULL;
        free(ithaca.voyages[i].file);
        ithaca.voyages[i].file = NULL;
        free(ithaca.voyages[i].destination);
        ithaca.voyages[i].destination = NULL;
    }
    free(ithaca.voyages);
    ithaca.voyages = NULL;
    ithaca.num_voyages = 0;
}

void signal_handler(int signum) {
   
    if (signum == SIGINT) {
        printMessage("\nIthaca closes the harbor.\n");
        freeIhaca();
        exit(0);
    }
}



int  loadIthaca (char *filepath, Ithaca *ithaca) {

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


int loadVoyages (char* filepath, Ithaca *ithaca) {

    int fd = open(filepath, O_RDONLY);

    if (fd < 0) {
        printError("Error opening voyages file\n");
        return -1;
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
                printError("Error is missing a field\n");
                close(fd);
                return -1 ;
            } else {
                
               ithaca->voyages = realloc(ithaca->voyages, sizeof(Voyage) * (ithaca->num_voyages + 1));
               
               ithaca->voyages[ithaca->num_voyages].object = object;
               ithaca->voyages[ithaca->num_voyages].file = file;
               ithaca->voyages[ithaca->num_voyages].destination = destination;
               ithaca->voyages[ithaca->num_voyages].reward = atoi(reward);
               ithaca->voyages[ithaca->num_voyages].id = ithaca->num_voyages + 1;
               free(reward);
               
               ithaca->num_voyages++;

            }

        }
        close(fd);
        return 0; 
    }
}

void printMessageInitialization(Ithaca *ithaca) {
    char *buffer;
    if (asprintf(&buffer, "Ithaca initialized. %d voyages loaded.\n", ithaca->num_voyages) == -1) {
        return;
    }
    
    printMessage(buffer);
    free(buffer);

    printMessage("Waiting for Odysseus...\n");
}



int main(int argc,char *argv[]){

    signal(SIGINT,signal_handler);
    
   
    if (argc!=3){
        printError("Error usage: ./ithaca ithaca.dat voyages.dat\n");
        return 1;
    } else {
        if (loadIthaca(argv[1],&ithaca) == -1) {
            printError("Error to load Ithaca configuration\n");
            freeIhaca();
            return 1;
        }
        if (loadVoyages(argv[2],&ithaca) == -1) {
            printError("Error to load Ithaca voyages\n");
            freeIhaca();
            return 1;
        }

        printMessageInitialization(&ithaca);
        while (1) {
            pause(); 
        }

        



    }

}