#define _GNU_SOURCE

#include "Files.h" 
#include "ithaca_config.h"
#include "trama.h"
#include "Connection.h"
#include "ithaca_actions.h" 


static int stopflag = 0;

void freeIthaca(Ithaca *ithaca) {
    free(ithaca->serverName);
    ithaca->serverName = NULL;
    free(ithaca->objectsFolder);
    ithaca->objectsFolder = NULL;
    free(ithaca->ip);
    ithaca->ip = NULL;
    ithaca->port = 0;
    for (int i = 0; i < ithaca->num_voyages; i++) {
        free(ithaca->voyages[i].object);
        ithaca->voyages[i].object = NULL;
        free(ithaca->voyages[i].file);
        ithaca->voyages[i].file = NULL;
        free(ithaca->voyages[i].destination);
        ithaca->voyages[i].destination = NULL;
        free(ithaca->voyages[i].assigned);
        ithaca->voyages[i].assigned = NULL;
    }
    free(ithaca->voyages);
    ithaca->voyages = NULL;
    ithaca->num_voyages = 0;
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        stopflag = 1; 
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
void initHandler(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);       
}

int WaitingForInput(int fd, fd_set *ready,fd_set *current,int timeout) {
    struct timeval timeout_struct;
    struct timeval *timeout_ptr = NULL;
    while(1){
        *ready = *current;
        if (timeout >= 0) {
            timeout_struct.tv_sec = timeout;
            timeout_struct.tv_usec = 0;
            timeout_ptr = &timeout_struct;
        } 
        int result = select(fd + 1, ready, NULL, NULL, timeout_ptr);
        if (result < 0) {
            if (errno == EINTR && stopflag == 0) {   
                continue;
            } else {
                return -1;
            }
        } 
       return result == 0 ? 0 : 1;
    }
}

void freeAll(char **names, fd_set *current, int max_fd, int fd_server) {
    for (int fd = 0; fd <= max_fd; fd++) {
        if (fd != fd_server && FD_ISSET(fd, current)) {
            close(fd);
        }
        free(names[fd]);
        names[fd] = NULL;
    }
    close(fd_server);
}

int main(int argc,char *argv[]){
    char *buffer;
    Ithaca ithaca={0};
    int fd_server;
    int fd_client;
    fd_set current, ready;
    int max_fd=-1;
    int result;
    char *names[FD_SETSIZE] = {NULL};
    Trama trama;
   


    signal (SIGPIPE, SIG_IGN);
    initHandler();
    
    if (argc!=3){
        printError("Usage: ./ithaca <config.dat> <voyages.dat>\n");
        return 1;
    } else {
        if (loadIthaca(argv[1],&ithaca) == -1) {
            printError("Error to load Ithaca configuration\n");
            freeIthaca(&ithaca);
            return 1;
        }
        if (loadVoyages(argv[2],&ithaca) == -1) {
            printError("Error to load Ithaca voyages\n");
            freeIthaca(&ithaca);
            return 1;
        }

       
        
        fd_server = createServerConnection(ithaca.port, ithaca.ip);
       
        if (fd_server < 0) {
                printError("ERROR: Ithaca could not open the harbor\n");
                freeIthaca(&ithaca);
                return 1;
        }

        printMessageInitialization(&ithaca);

        max_fd = fd_server;
        FD_ZERO(&current);
        FD_SET(fd_server, &current);
        
        while (!stopflag) {
            if ((result = WaitingForInput(max_fd, &ready, &current, -1)) < 0) {
                break;
            }

            for (int j = 0; j <= max_fd; j++) {
                if (!FD_ISSET(j, &ready)) {
                    continue;
                }
                if ((j == fd_server)){
                        if ((fd_client = accept(fd_server, NULL, NULL)) < 0) {
                            printError("ERROR: Cloud not accept the Odysseus\n");
                            continue;
                        } else {
                            FD_SET(fd_client, &current);
                            if (fd_client > max_fd) {
                                max_fd = fd_client;
                            }
                        }
                } else {
                    if (recibir_trama(j, &trama) < 0) {
                        if (asprintf(&buffer, ">>> %s lost connection with Ithaca.\n", names[j]) != -1) {
                            printMessage(buffer);
                            free(buffer);
                        }
                        free(names[j]);
                        names[j] = NULL;
                        FD_CLR(j, &current);
                        close(j);
                        if (j == max_fd) {
                            while (max_fd >= 0 && !FD_ISSET(max_fd, &current)) {
                                max_fd--;
                            }
                        }
                        continue;
                    } else {
                        detectTrama(j, &trama, &ithaca, names, &current, &max_fd);
                    }

                }
            }


        }
        freeAll(names, &current, max_fd, fd_server);
        if (asprintf(&buffer, "\nIthaca closes the harbor.\n") != -1) {
            printMessage(buffer);
            free(buffer);
        }
        freeIthaca(&ithaca);
        return 0;
    }

}