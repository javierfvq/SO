#define _GNU_SOURCE

#include "island_actions.h" 

static int  stopflag = 0;

void free_island(Island_CONFIG *island) {


    free(island->name);
    island->name = NULL;
   

    free(island->folder);
    island->folder = NULL;
    free(island->island_ip);
    island->island_ip = NULL;
    

    for(int i=0; i<island->counter; i++){
        free(island->routes[i].name);
        island->routes[i].name = NULL;
        free(island->routes[i].ip);
        island->routes[i].ip = NULL;
    }
    
    free(island->routes);
    island->routes = NULL;
    free(island->products);
    island->products = NULL;
    island->product_count=0;
    island->counter=0;

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

void signal_handler(int signum) {
    if (signum == SIGINT) {
        stopflag = 1; 
    }
}


void printMessageInitialization(Island_CONFIG *island) {
    char *buffer;
    if (asprintf(&buffer, "\nIsland %s initialized.\n", island->name) == -1) {
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

    if (island->counter == 1) {
        if (asprintf(&buffer, "%d sea route loaded.\n", island->counter) == -1) {
            return;
        }
        printMessage(buffer);
        free(buffer);
    } else{
        if (asprintf(&buffer, "%d sea routes loaded.\n", island->counter) == -1) {
            return;
        }
        printMessage(buffer);
        free(buffer);
    }
    
    if (asprintf(&buffer, "%d products available.\n", island->product_count) == -1) {
        return;
    }
    printMessage(buffer);
    free(buffer);
 
}
void initHandler(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);       
}


int main(int argc,char *argv[]){
    char *buffer;
    Island_CONFIG island={0};
    int fd_servidor;
    int fd_cliente;
    fd_set current;
    fd_set ready;
    int max_fd = -1;
    int result;
    Port port = {0}; 
    Trama trama;
    
    initHandler();
    signal(SIGPIPE, SIG_IGN);

    if (argc!=3){
        printError("Usage: ./island <config.dat> <stock.db>\n");
        return 1;
    }else{
        if (load_config_island(argv[1],&island) == -1) {
            free_island(&island);
            printError("Error to load island configuration\n");
            return 1;
        }
        island.stock_path = argv[2];
        if (load_Binary(island.stock_path ,&island) == -1) {
            free_island(&island);
            printError("Error to load stock binary file\n");
            return 1;
        }
        
        QUEUE_init(&port.queue);
        fd_servidor = createServerConnection(island.island_port, island.island_ip);
           
        if (fd_servidor < 0) {
                    printError("ERROR: The island couldn't open his port\n");
                    free_island(&island);
                    return 1;
        }

        printMessageInitialization(&island);

        max_fd = fd_servidor;
        FD_ZERO(&current);
        FD_SET(fd_servidor, &current);

        while(!stopflag){
            if ((result = WaitingForInput(max_fd, &ready, &current, -1)) < 0) {
                break;
            }

            for (int j = 0; j <= max_fd; j++) {
                if (!FD_ISSET(j, &ready)) {
                    continue;
                }

                if (j == fd_servidor) {
                    if ((fd_cliente = accept (fd_servidor, NULL, NULL)) < 0) {
                        printError("ERROR: Could not accept the Odyseus\n");
                        continue;
                    } else {
                        FD_SET(fd_cliente, &current);
                        if (fd_cliente > max_fd) {
                            max_fd = fd_cliente;
                        }
                    }
                } else {
                    if (recibir_trama(j, &trama) < 0) {
                        if (port.status[j] != NO_SHIP) {
                        if (asprintf(&buffer, ">>> %s has left\n", port.names[j]) != -1) {
                            printMessage(buffer);
                            free(buffer);
                        }
                        abandonaPuerto (j, &port);
                    }

                        
                        free(port.names[j]);
                        port.names[j] = NULL;
                        close(j);
                        FD_CLR(j, &current);
                        
                        if (j == max_fd) {
                            while (max_fd >= 0 && !FD_ISSET(max_fd, &current)) {
                                max_fd--;
                            }
                        }       

                        continue;
                    } else {
                       detectTrama(j, &trama, &island, &port);
                }

                }
                
            }
        }
        freeAll(port.names, &current, max_fd, fd_servidor);
        if (asprintf(&buffer, "\n%s closes its port.\n", island.name) != -1) {
            printMessage(buffer);
            free(buffer);
        }
        free_island(&island);


        return 0;


    }

}