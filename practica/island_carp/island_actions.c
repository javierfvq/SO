
#define _GNU_SOURCE       
#include "island_actions.h"
#include "market.h"



void abandonaPuerto (int fd, Port *port) {
    char *buffer;
    int next;
    if (port->status[fd] == DOCKED) {
        port->count_docked = (port -> count_docked) -1;
        port->status[fd] = NO_SHIP;

        if (!QUEUE_isEmpty(&port->queue)) {
            next = QUEUE_get(&port->queue);
            port->status[next] = DOCKED;
            port->count_docked++;
            
            sendResponse(next, ARRIVE_TYPE, OK_FLAG, "DOCKED");

            if (asprintf(&buffer, ">>> DOCKED sent to %s; he may enter the port.\n", port->names[next]) != -1) {
                printMessage(buffer);
                free(buffer);
            }

        }

    }else { 
        if (port->status[fd] == WAITING) {
            QUEUE_remove(&port->queue, fd);
            port->status[fd] = NO_SHIP;
        }
    }

}

void doLeave (int fd, Port *port) {

    char *buffer;
    if (asprintf(&buffer, "\n>>> %s has left.\n", port->names[fd]) != - 1) {
        printMessage(buffer);
        free(buffer);
    }
    sendResponse(fd, LEAVE_TYPE, OK_FLAG, "OK");
    abandonaPuerto(fd, port);

}


void doArrive (int fd, Trama *trama, Island_CONFIG *island, Port *port) {

    int longitud = 0;
    char *buffer;

    if (port->names[fd] != NULL) {
        sendResponse(fd, ARRIVE_TYPE, ERROR_FLAG, "ARLEADY_IN_THE_PORT");
        return;
    }

    if ((longitud = strlen(trama->data)) == 0) {
        sendResponse(fd, ARRIVE_TYPE, ERROR_FLAG, "INVALID_NAME");
        return;
    }

    port->names[fd] = (char *)malloc(longitud + 1);
    if (port->names[fd] == NULL) {
        sendResponse(fd, ARRIVE_TYPE, ERROR_FLAG, "SERVER_ERROR");
        return;
    } 
   
    strcpy(port -> names[fd], trama->data);

    if (asprintf(&buffer, "\n>>> %s approaching %s\n",port->names[fd], island->name) == -1) {   
        sendResponse(fd, ARRIVE_TYPE, ERROR_FLAG, "SERVER_ERROR");
        return;
    }
    printMessage(buffer);
    free(buffer);
  
    
    if (port -> count_docked < island -> max_capacity) {
        port -> count_docked++;
        port->status[fd] = DOCKED;
        if (asprintf(&buffer, ">>> %s entered  the port\n\n",port->names[fd]) == -1) {   
            sendResponse(fd, ARRIVE_TYPE, ERROR_FLAG, "SERVER_ERROR");
            return;
        }
        printMessage(buffer);
        free(buffer);
        sendResponse(fd, ARRIVE_TYPE, OK_FLAG, "DOCKED");
    } else {
        if (QUEUE_add(&port->queue, fd) == QUEUE_OK) {
            port->status[fd] = WAITING;
            if (asprintf(&buffer, ">>> Port full. %s waiting offshore in the FIFO queue.\n\n",port->names[fd]) == -1) {   
                sendResponse(fd, ARRIVE_TYPE, ERROR_FLAG, "SERVER_ERROR");
                return;
            }
            printMessage(buffer);
            free(buffer);
            sendResponse(fd, ARRIVE_TYPE, OK_FLAG, "WAIT");
        } else {
            sendResponse(fd, ARRIVE_TYPE, ERROR_FLAG, "QUEUE_FULL");
        }

    }
    
}

void doListMarket (int fd, Island_CONFIG *island, Port *port) {

    int total = 0;
    char *buffer;

    if (port->status[fd] != DOCKED) {  
            sendResponse(fd, LIST_MARKET_TYPE, ERROR_FLAG, "NOT_DOCKED");
            return;
    }
    total = island->product_count + 1;

    for (int i = 0; i < total; i++) {
        buffer = ListProductsInMarket(island, i);

        if (buffer == NULL){
            sendResponse(fd, LIST_MARKET_TYPE, ERROR_FLAG, "SERVER_ERROR");
            return;
        }

        sendResponse(fd, LIST_MARKET_TYPE, OK_FLAG, buffer);
        free(buffer);
    }

}



void doBuyMap (int fd, Trama *trama, Island_CONFIG *island, Port *port) {
    char *buffer;

        if (port->status[fd] != DOCKED) {  
            sendResponse(fd, BUY_MAP_TYPE, ERROR_FLAG, "NOT_DOCKED");
            return;
        }

        if (strcmp(trama->data , "1") != 0) {
            sendResponse(fd, BUY_MAP_TYPE, ERROR_FLAG, "INVALID_AMOUNT");
            return;
        }


        if (island->counter == 0) {
            sendResponse(fd, BUY_MAP_TYPE, OK_FLAG, "OK&50&0");
        } else {
            for (int i = 0; i < island->counter; i++) {
                buffer = buyMapInMarket(island, i);
                if (buffer == NULL) {
                    sendResponse(fd, BUY_MAP_TYPE, ERROR_FLAG, "SERVER_ERROR");
                    return;
                }
                sendResponse(fd, BUY_MAP_TYPE, OK_FLAG, buffer);
                free(buffer);
            }

        }

        if (asprintf(&buffer, ">>> %s bought the local map\n", port->names[fd]) != -1) {
            printMessage(buffer);
            free(buffer);
        }
}

void doBuy (int fd, Trama *trama, Island_CONFIG *island, Port *port) {
    char *buffer;
    char *trozos[2];
    int quantity;
    int price;
    int result;
    int index;

    if (port->status[fd] != DOCKED) {
        sendResponse(fd,BUY_TYPE, ERROR_FLAG, "NOT_DOCKED");
        return;
    }

    if (partir_datos(trama->data, trozos, 2, '&') != 2) {
        sendResponse(fd,BUY_TYPE, ERROR_FLAG, "INCORRECT_DATA");
        return; 
    }

    if ((quantity = atoi_personalizado(trozos[1])) == -1) {
        sendResponse(fd, BUY_TYPE, ERROR_FLAG, "INCORRECT_AMOUNT");
        return;
    }

    
    result = BuyProductInMarket(island, trozos[0] , quantity, &price);

    switch (result) {

        case MARKET_OK:
        index = searchProductInMarket(island, trozos[0]);
        if (asprintf(&buffer, "OK&%s&%d&%d", island->products[index].name, quantity, price) != -1) {
            sendResponse(fd, BUY_TYPE, OK_FLAG, buffer);
            free(buffer);
            
            if (asprintf(&buffer, ">>> %s bought %d kg of %s.\n", port->names[fd], quantity, island->products[index].name) != -1) {
                printMessage(buffer);
                free(buffer);

            }
        }
        break;

        case BUY_ERROR_NOT_ENOUGH_STOCK:
            sendResponse(fd, BUY_TYPE, ERROR_FLAG, "NOT_ENOUGH_STOCK");
            return;
        break;

        case BUY_ERROR_PRODUCT_NOT_FOUND:
            sendResponse(fd, BUY_TYPE, ERROR_FLAG, "PRODUCT_NOT_FOUND");
            return;
        break;

    }
}



void doSell (int fd, Trama* trama, Island_CONFIG *island, Port *port) {

    char *buffer;
    int index;
    int price;
    int cantidad;
    int result;
    char *trozos[2];

    if (port->status[fd] != DOCKED) {
        sendResponse(fd, SELL_TYPE, ERROR_FLAG, "NOT_DOCKED");
        return;
    }

    if (partir_datos( trama->data,trozos ,2,'&') != 2) {
        sendResponse(fd, SELL_TYPE, ERROR_FLAG, "INCORRECT_DATA");
        return;
    }

    if ((cantidad = atoi_personalizado(trozos[1])) == -1) {
        sendResponse(fd, SELL_TYPE, ERROR_FLAG, "INCORRECT_AMOUNT");
        return;
    }

    result = SellProductInMarket(island, trozos[0], cantidad, &price);


    switch (result) {
        
        case MARKET_OK:
        index = searchProductInMarket(island, trozos[0]);
        if (asprintf(&buffer, "OK&%s&%d&%d", island->products[index].name, cantidad, price) != -1) {
            sendResponse(fd, SELL_TYPE, OK_FLAG, buffer);
            free(buffer);

            if (asprintf(&buffer, ">>> %s sold %d kg of %s.\n", port->names[fd],cantidad,island->products[index].name) != -1) {
                printMessage(buffer);
                free(buffer);
            }
        }
        
        break;

        case SELL_ERROR_PRODUCT_NOT_FOUND:
        sendResponse(fd, SELL_TYPE, ERROR_FLAG, "PRODUCT_NOT_FOUND");
        break;
    }
}

void detectTrama (int fd, Trama *trama, Island_CONFIG *island, Port *port){
    
    if (trama ->flags != REQUEST_FLAG) {
        sendResponse(fd, NACK_TYPE, ERROR_FLAG, "INVALID_FLAGS");
        return;
    }

    if (port->names[fd] == NULL && trama->type != ARRIVE_TYPE) {
        sendResponse(fd, trama->type, ERROR_FLAG, "NOT_IDENTIFIED");
        return;
    }

       switch (trama->type) {

        case ARRIVE_TYPE:
            doArrive(fd, trama, island, port);
            break;

        case LEAVE_TYPE:
            doLeave(fd, port);
            break;
        case LIST_MARKET_TYPE:
            doListMarket(fd, island, port);
            break;

        case BUY_TYPE:
            doBuy (fd, trama, island, port);
            break;
        case SELL_TYPE:
            doSell(fd, trama, island, port);
            break;

        case BUY_MAP_TYPE:
            doBuyMap(fd, trama, island, port);
            break;

        default:
            sendResponse(fd, NACK_TYPE, ERROR_FLAG, "INVALID_TYPE");
            break;
    }
}