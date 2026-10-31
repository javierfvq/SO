#define _GNU_SOURCE
#include "ithaca_actions.h"



void doConnect (int fd, Trama *trama, char **names) {
    char *buffer;

    if (names[fd] != NULL) {
        sendResponse (fd, CONNECT_TYPE, ERROR_FLAG, "ALREADY_CONECTED");
        return;
    }

    if (trama->length == 0) {
        sendResponse(fd, CONNECT_TYPE, ERROR_FLAG, "INVALID_NAME");
        return;
    }

    names[fd] = (char*)malloc(strlen(trama->data) + 1);
    if (names[fd] == NULL) {
        sendResponse(fd, CONNECT_TYPE, ERROR_FLAG, "SERVER_ERROR");
        return;
    } else {
        strcpy(names[fd], trama->data);
    }

    sendResponse(fd, CONNECT_TYPE, OK_FLAG, "OK");
    if (asprintf(&buffer, "\n>>> %s connected to Ithaca.\n", names[fd]) != -1) {
        printMessage(buffer);
         free(buffer);   
    }

}

void seeFailedVoyages (Ithaca *ithaca, char * name) {

    char *buffer;

    for (int i = 0; i < ithaca->num_voyages; i++) {
        if (ithaca->voyages[i].status == ASSIGNED && strcmp(name, ithaca->voyages[i].assigned) == 0){
            ithaca->voyages[i].status = FAILED;
            if (asprintf(&buffer, ">>> Voyage %d of %s marked as failed.\n", ithaca->voyages[i].id, ithaca->voyages[i].assigned) != -1) {
                printMessage(buffer);
                 free(buffer);   
            }
        }
    }
}

void doListVoyages (int fd, Ithaca *ithaca, char *name) {
    char *buffer;
    int total_availables = 0;
    int i = 0;

    if (asprintf(&buffer, ">>> %s requested available voyages.\n", name) != -1) {
        printMessage(buffer);
         free(buffer);   
    }

    seeFailedVoyages(ithaca, name);

    for (int i = 0; i < ithaca->num_voyages; i++) {
        if (ithaca -> voyages[i].status == AVAILABLE) {
            total_availables++;
        }
    }

    if (total_availables == 0) {
        sendResponse(fd, LIST_VOYAGES_TYPE, OK_FLAG, "0");
        return;
    }

    for (int j = 0; j < ithaca->num_voyages; j++) {

        if (ithaca->voyages[j].status == AVAILABLE) {
            i++;
            if  (asprintf(&buffer, "%d&%d&%d&%s&%s&%d", i, total_availables, ithaca->voyages[j].id, ithaca->voyages[j].object, ithaca->voyages[j].destination,
            ithaca->voyages[j].reward) != -1) {
                sendResponse(fd, LIST_VOYAGES_TYPE, OK_FLAG, buffer); 
                free(buffer);
                continue;
            }

            sendResponse(fd, LIST_VOYAGES_TYPE, ERROR_FLAG, "SERVER_ERROR");
            return;

                
        }

    }

}


void doAccept (int fd,Trama *trama, Ithaca *ithaca, char *name) {
    int id_voyage = atoi_personalizado(trama->data);
    char *buffer;
    int index = id_voyage - 1;

    if (id_voyage < 1 || id_voyage > ithaca -> num_voyages) {
        sendResponse(fd, ACCEPT_TYPE, ERROR_FLAG, "INVALID_VOYAGE");
        return;
    }


    for (int i = 0; i < ithaca -> num_voyages; i++) {
        if (ithaca->voyages[i].status == ASSIGNED && (strcmp (ithaca->voyages[i].assigned, name) == 0)) {
            sendResponse(fd, ACCEPT_TYPE, ERROR_FLAG, "ALREADY_HAS_VOYAGE");
            return;
        }
    }

    if (ithaca->voyages[index].status != AVAILABLE) {
        sendResponse(fd, ACCEPT_TYPE, ERROR_FLAG, "VOYAGE_NOT_AVAILABLE");
        return;
    }

    ithaca -> voyages[index].assigned = (char*)malloc(strlen(name)+1);

    if (ithaca -> voyages[index].assigned == NULL) {
        sendResponse(fd, ACCEPT_TYPE, ERROR_FLAG, "SERVER_ERROR");
     
    } else {
        strcpy(ithaca->voyages[index].assigned, name);
        ithaca->voyages[index].status = ASSIGNED;
        if (asprintf(&buffer, "OK&%d&%s&%s&%d", ithaca->voyages[index].id, ithaca->voyages[index].object,ithaca->voyages[index].destination, ithaca->voyages[index].reward) != -1) {
            sendResponse(fd, ACCEPT_TYPE, OK_FLAG, buffer);
            free(buffer);
            if (asprintf(&buffer, ">>> Voyage %d assigned to %s.\n", id_voyage, name) != -1) {
                printMessage(buffer);
                free(buffer);
            }
        } else {
            sendResponse(fd, ACCEPT_TYPE, ERROR_FLAG, "SERVER_ERROR");
        }
    }
}

void doDisconnect (int fd, char **names, fd_set *current, int *max_fd) {

    sendResponse(fd, DISCONNECT_TYPE, OK_FLAG, "OK");
    char* buffer;
    if (asprintf(&buffer, ">>> %s has left Ithaca. Fair Winds!\n", names[fd]) != -1 ) {
        printMessage(buffer);
        free(buffer);
    }

    free(names[fd]);
    names[fd] = NULL;

    FD_CLR(fd, current);
    close(fd);

    if (fd == *max_fd) {
        while (*max_fd >= 0 && !FD_ISSET(*max_fd, current)) {
            (*max_fd)--;
        }
    }
}

void detectTrama (int fd, Trama *trama, Ithaca *ithaca, char **names, fd_set *current, int *max_fd) {

    if (trama -> flags != REQUEST_FLAG) {
        sendResponse(fd, NACK_TYPE, ERROR_FLAG, "INVALID_FLAGS");
        return;
    }

    if (names[fd] == NULL && trama -> type != CONNECT_TYPE) {
        sendResponse(fd, trama->type, ERROR_FLAG, "NOT_IDENTIFIED");
        return;
    }

    switch (trama->type) {

        case CONNECT_TYPE:
            doConnect(fd, trama, names);
        break;
            
        case LIST_VOYAGES_TYPE:
            doListVoyages (fd, ithaca, names[fd]);
        break;

        case ACCEPT_TYPE:
            doAccept(fd, trama, ithaca, names[fd]);
        break;
        case DISCONNECT_TYPE:
            doDisconnect(fd, names, current, max_fd);
        break;
        default:
            sendResponse(fd, NACK_TYPE, ERROR_FLAG, "INVALID_TYPE");
            break;
    }



}

