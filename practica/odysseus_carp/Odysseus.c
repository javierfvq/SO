#define _GNU_SOURCE

#include "odysseus_config.h"
#include "terminal.h"

#define TECLADO 0
    
static   int stopflag = 0;
static   int eatFlag = 0;


void freeOdysseus(Odysseus *odysseus) {
    if (odysseus == NULL) {
        return; 
    }
    free(odysseus->name);
    odysseus->name = NULL;
    free(odysseus->folder);
    odysseus->folder = NULL;
    free(odysseus->itaca_ip);
    odysseus->itaca_ip = NULL;
    free(odysseus->ip_start);
    odysseus->ip_start = NULL;
    odysseus->port_start = 0;
    odysseus->money = 0;
    for (int i = 0; i < odysseus->food_count; i++) {
        free(odysseus->food[i].name);
        odysseus->food[i].name = NULL;
    }
    odysseus->food_count = 0;
    free(odysseus->food);
    odysseus->food = NULL;
    if (odysseus->viaje_actual.fd != -1) {
        close(odysseus->viaje_actual.fd);
        odysseus->viaje_actual.fd = -1;
    }

    free(odysseus->viaje_actual.isla_actual);
    odysseus->viaje_actual.isla_actual = NULL;

    free(odysseus->viaje_actual.encargo_actual.objeto);
    odysseus->viaje_actual.encargo_actual.objeto = NULL;

    free(odysseus->viaje_actual.encargo_actual.destino);
    odysseus->viaje_actual.encargo_actual.destino = NULL;
    freeMapa(&odysseus->mapa);
    odysseus->viaje_actual.ubicacion = MAR;
}

void handlerOdysseus(int signum){
    if (signum == SIGINT) {
        stopflag = 1; 
    }else if (signum == SIGALRM) {
        eatFlag = 1; 
    }
}

int WaitingForInputOdysseus(int fd, fd_set *ready,fd_set *current,int timeout) {
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
            if (errno == EINTR && stopflag == 0 && eatFlag == 0) {   
                continue;
            } else {
                return -1;
            }
        } 
       return result == 0 ? 0 : 1;
    }
}

void printMessageInitialization (char *nombre){
    char *buffer;
    if (asprintf(&buffer, "Odysseus %s is ready to sail.\n\n", nombre) == -1) {
        return;
    }
    printMessage(buffer);
    free(buffer);
}

void initHandler(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handlerOdysseus;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);  
    sigaction(SIGALRM, &sa, NULL);   
}

void procesarTramaPuerto(Odysseus *odysseus,Trama *trama){
    char *buffer;
    if (trama->type == ARRIVE_TYPE && strcmp(trama->data, "DOCKED") == 0){
        alarm(0);
        odysseus->viaje_actual.ubicacion = EN_PUERTO;
        asprintf(&buffer, ">>> %s has granted access to the port.\n",
        odysseus->viaje_actual.isla_actual);
        printMessage(buffer);
        free(buffer);
        comprobarLlegada(odysseus);
    }
}

int main (int argc, char* argv[]) {
    Odysseus odysseus = {0};
    odysseus.viaje_actual.fd = -1;
    char *buffer;
    fd_set current;
    fd_set ready;
    int max_fd=-1;
    int result;
    int total_food = 0;
    

    signal(SIGPIPE, SIG_IGN);
    initHandler();

    if (argc != 2) {
        printError("Usage: ./odysseus <config.dat>\n");
        return 1;
    } else {
        if (load_odysseus(argv[1], &odysseus) == -1) {
            freeOdysseus(&odysseus);
            printError("Error to load odysseus configuration\n"); 
            return 1;
        } 
    }
    printMessageInitialization(odysseus.name);
    
    printMessage("$ ");
    while (!stopflag){
        FD_ZERO(&current);
        FD_SET(0, &current);
        max_fd = 0;
        if(odysseus.viaje_actual.fd != -1){
            FD_SET(odysseus.viaje_actual.fd, &current);
            if (odysseus.viaje_actual.fd > max_fd) {
                max_fd = odysseus.viaje_actual.fd;
            }
        }
        result = WaitingForInputOdysseus(max_fd, &ready, &current, -1);
        if(eatFlag == 1){
            eatFlag = 0;
            if (eatProvisional(&odysseus,7) == -1) {
                printMessage("The crew has run out of provisions.\n");
                asprintf(&buffer, "Odysseus %s has been lost at sea.\n", odysseus.name);
                printMessage(buffer);
                free(buffer);
                break;
            }else{
                total_food = totalFoodWeight(&odysseus);
                printMessage(">>> The crew consumes another ration.\n");
                asprintf(&buffer, "Provisions remaining: %d kg\n\n", total_food);
                printMessage(buffer);
                free(buffer);
                if (odysseus.viaje_actual.ubicacion == ESPERANDO) {
                alarm(5);
                }       
            }
        }
        if (result < 0) {
            if(stopflag){
                break;
            }
            continue;
        }
        for (int j = 0; j <= max_fd; j++) {
            if (!FD_ISSET(j, &ready)) {
                continue;
            }
            if (j == TECLADO) {
                char *linea = leer_respuesta();
                if (linea == NULL) {
                    stopflag = 1;
                    break;
                }
                DetectCommandType(linea, &odysseus);
                free(linea);
                printMessage("$ ");
            }else if (j == odysseus.viaje_actual.fd) {
                Trama trama;
                if (recibir_trama(odysseus.viaje_actual.fd, &trama) < 0) {
                    printError("Error: Could not receive response from island.\n");
                    cerrarConexionOdysseus(&odysseus);
                    break;
                }
                procesarTramaPuerto(&odysseus, &trama);
            }
        }
    }
    doDisconnect(&odysseus);
    if (asprintf(&buffer, "\n Odysseus is leaving.\n") != -1) {
        printMessage(buffer);
        free(buffer);
    }
    freeOdysseus(&odysseus);
    return 0;
        
}