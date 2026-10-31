#include "OdysseusActions.h"

void cerrarConexionOdysseus(Odysseus *odysseus){
    if (odysseus->viaje_actual.fd != -1) {
        close(odysseus->viaje_actual.fd);
        odysseus->viaje_actual.fd = -1;
        odysseus->viaje_actual.ubicacion = MAR;
    }
}


void doConnect(Odysseus *odysseus) {
    char *buffer;
    Trama respuesta;
    if (odysseus->viaje_actual.fd != -1) {
        printError("Error: Already connected to Ithaca.\n");
        return;
    }
    odysseus->viaje_actual.fd = createClientConnection(odysseus->itaca_port, odysseus->itaca_ip);
    if (odysseus->viaje_actual.fd == -1) {
        printError("Error: Could not connect to Ithaca.\n");
        return ;
    }

    if (sendResponse(odysseus->viaje_actual.fd, CONNECT_TYPE, REQUEST_FLAG, odysseus->name) < 0) {
        printError("Error: Could not send connection request to Ithaca.\n");
        cerrarConexionOdysseus(odysseus);
        return;
    }
   
    if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from Ithaca.\n");
        cerrarConexionOdysseus(odysseus);
        return;
    }
    if (respuesta.type != CONNECT_TYPE) {
        printError("Error: Invalid response type from Ithaca.\n");
        cerrarConexionOdysseus(odysseus);
        return;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        cerrarConexionOdysseus(odysseus);
        free(buffer);
        return;
    }
    odysseus->viaje_actual.ubicacion = ITHACA;
    printMessage("Connected to Ithaca.\n\n");

}


void DoStatus(Odysseus *odysseus){
    char *buffer;
    char *name = malloc(strlen(odysseus->name) + 1);
    if (name == NULL) {
        printError("Error allocating memory for name\n");
        return;
    }
    strcpy(name, odysseus->name);

    to_upper_case(name);
    if (asprintf(&buffer,"--- %s ---\n",name) == -1) {
        return;
    }
    free(name);
    printMessage(buffer);
    free(buffer);

    asprintf(&buffer,"Gold: %d\n\n",odysseus->money);
    printMessage(buffer);
    free(buffer);

    printMessage("Cargo:\n");
    for (int i = 0; i < odysseus->food_count; i++) {
        asprintf(&buffer, "%s: %d kg\n", odysseus->food[i].name, odysseus->food[i].quantity);
        printMessage(buffer);
        free(buffer);
    }
    asprintf(&buffer, "Total provisions: %d kg\n\n", totalFoodWeight(odysseus));
    printMessage(buffer);
    free(buffer);

    if (odysseus->viaje_actual.encargo_actual.objeto == NULL || odysseus->viaje_actual.encargo_actual.destino == NULL) {
        printMessage("Current voyage: None\n\n");
    }else{
        asprintf(&buffer,"Current voyage: %s -> %s\n",odysseus->viaje_actual.encargo_actual.objeto, odysseus->viaje_actual.encargo_actual.destino);
        printMessage(buffer);
        free(buffer);

        asprintf(&buffer,"Reward: %d\n\n", odysseus->viaje_actual.encargo_actual.recompensa);
        printMessage(buffer);
        free(buffer);
    }

    switch(odysseus->viaje_actual.ubicacion){
        case MAR:
            printMessage("Current location: At sea\n\n");
            break;
        case ITHACA:
            printMessage("Current location: Ithaca\n\n");
            break;
        case ESPERANDO:
            asprintf(&buffer,"Current location: Waiting in port %s\n\n", odysseus->viaje_actual.isla_actual);
            printMessage(buffer);
            free(buffer);
            break;
        case EN_PUERTO:
            asprintf(&buffer,"Current location: %s\n\n", odysseus->viaje_actual.isla_actual);
            printMessage(buffer);
            free(buffer);
            break;
        case VIAJANDO:
            printMessage("Current location: Traveling to destination\n\n");
            break;
    }
}

void DoListVoyages(Odysseus *odysseus) {
    char *buffer;
    Trama respuesta;
    char *trozos[8];
    int total_availables = 0;

    if (odysseus->viaje_actual.ubicacion != ITHACA) {
    printMessage("You must be connected to Ithaca.\n\n");
    return;
    }
    if (odysseus->viaje_actual.encargo_actual.id != 0) {
    printMessage("You already have an active voyage.\n\n");
    return;
    }

    if (sendResponse(odysseus->viaje_actual.fd, LIST_VOYAGES_TYPE, REQUEST_FLAG, "") < 0) {
        printError("Error: Could not send list voyages request to Ithaca.\n");
        cerrarConexionOdysseus(odysseus);  
        return;
    }

    if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from Ithaca.\n");
        cerrarConexionOdysseus(odysseus);  
        return;
    }
    if (respuesta.type != LIST_VOYAGES_TYPE) {
        printError("Error: Invalid response type from Ithaca.\n");
        return;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        return;
    }

    int num_trozos = partir_datos(respuesta.data, trozos, 8, '&');

    if (num_trozos < 2) {
        printMessage("No available voyages at the moment.\n\n");
        return;
    }
    if (num_trozos < 6) {
    printError("Error: malformed voyage entry.\n");
    return;
    }

    total_availables = atoi(trozos[1]);
    
    printMessage("--- AVAILABLE VOYAGES ---\n");
    asprintf(&buffer, "%s. %s -> %s [%s]\n", trozos[2], trozos[3], trozos[4], trozos[5]);  
    printMessage(buffer);
    free(buffer);

    for (int i = 1; i < total_availables; i++) {
        if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
            printError("Error: Could not receive response from Ithaca.\n");
             cerrarConexionOdysseus(odysseus);  
            return;
        }
        if (respuesta.type != LIST_VOYAGES_TYPE) {
            printError("Error: Invalid response type from Ithaca.\n");
            return;
        }
        if(respuesta.flags == ERROR_FLAG){
            asprintf(&buffer,"%s\n\n",respuesta.data);
            printMessage(buffer);
            free(buffer);
            return;
        }
        num_trozos = partir_datos(respuesta.data, trozos, 8, '&');
        asprintf(&buffer, "%s. %s -> %s [%s]\n", trozos[2], trozos[3], trozos[4], trozos[5]);   
        printMessage(buffer);
        free(buffer);
    }
    printMessage("\n");
}
void DoAccept(Odysseus *odysseus,int id_viaje){
    char *buffer;
    Trama respuesta;
    char *trozos[5];
    int num_trozos = 0;

    if (odysseus->viaje_actual.ubicacion != ITHACA) {
    printMessage("You must be connected to Ithaca.\n\n");
    return;
    }

    if (odysseus->viaje_actual.encargo_actual.id != 0) {
    printMessage("You already have an active voyage.\n\n");
    return;
    }
    asprintf(&buffer, "%d", id_viaje);
    if (sendResponse(odysseus->viaje_actual.fd, ACCEPT_TYPE, REQUEST_FLAG, buffer) < 0) {
        printError("Error: Could not send accept voyage request to Ithaca.\n");
        cerrarConexionOdysseus(odysseus);  
        free(buffer);
        return;
    }
    free(buffer);
    if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from Ithaca.\n");
        cerrarConexionOdysseus(odysseus);  
        return;
    }
    if(respuesta.type != ACCEPT_TYPE) {
        printError("Error: Invalid response type from Ithaca.\n");
        return;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        return;
    }

    num_trozos = partir_datos(respuesta.data, trozos, 5, '&');
    if (num_trozos < 5) {
        printError("Error:incorrect trama format from Ithaca.\n");
        return;
    }
    odysseus->viaje_actual.encargo_actual.id = atoi(trozos[1]);
    odysseus->viaje_actual.encargo_actual.objeto = malloc(strlen(trozos[2]) + 1);
    if (odysseus->viaje_actual.encargo_actual.objeto == NULL) {
        printError("Error allocating memory for object name\n");
        return;
    }
    strcpy(odysseus->viaje_actual.encargo_actual.objeto, trozos[2]);
    odysseus->viaje_actual.encargo_actual.destino = malloc(strlen(trozos[3]) + 1);
    if (odysseus->viaje_actual.encargo_actual.destino == NULL) {
        printError("Error allocating memory for destination name\n");
        return;
    }
    strcpy(odysseus->viaje_actual.encargo_actual.destino, trozos[3]);
    odysseus->viaje_actual.encargo_actual.recompensa = atoi(trozos[4]);

    asprintf(&buffer,"Voyage %d accepted.\n", odysseus->viaje_actual.encargo_actual.id);
    printMessage(buffer);
    free(buffer);

    asprintf(&buffer,"Destination: %s.\n\n", odysseus->viaje_actual.encargo_actual.destino);
    printMessage(buffer);
    free(buffer);
}


void doDisconnect(Odysseus *odysseus){
    char *buffer;
    Trama respuesta;
    char tipo;

    if (odysseus->viaje_actual.fd == -1) {
        return;
    }
    tipo = odysseus->viaje_actual.ubicacion == ITHACA ? DISCONNECT_TYPE : LEAVE_TYPE;
    if (sendResponse(odysseus->viaje_actual.fd, tipo, REQUEST_FLAG, "") < 0) {
        printError("Error: Could not send disconnect request to Ithaca.\n");
        cerrarConexionOdysseus(odysseus);
        return;
    }

    if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from Ithaca.\n");
        cerrarConexionOdysseus(odysseus);  
        return;
    }
    if(respuesta.type != tipo) {
        printError("Error: Invalid response type from Ithaca.\n");
        cerrarConexionOdysseus(odysseus);
        return;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        cerrarConexionOdysseus(odysseus);
        return;
    }
    cerrarConexionOdysseus(odysseus);
}


void DoListMarket(Odysseus *odysseus){
    char *buffer;
    Trama respuesta;
    char *trozos[5];
    char *mayus_name;
    

    if (odysseus->viaje_actual.ubicacion != EN_PUERTO) {
        printMessage("You must be docked at an island to list the market.\n\n");
        return;
    }
    mayus_name = malloc(strlen(odysseus->viaje_actual.isla_actual) + 1);
    if (mayus_name == NULL) {
        printError("Error allocating memory for island name\n");
        return;
    }
    strcpy(mayus_name, odysseus->viaje_actual.isla_actual);
    to_upper_case(mayus_name);

    if (sendResponse(odysseus->viaje_actual.fd, LIST_MARKET_TYPE, REQUEST_FLAG, "") < 0) {
        printError("Error: Could not send list market request to island.\n");
        free(mayus_name);
        return;
    }

    if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from island.\n");
        free(mayus_name);
        return;
    }
    if (respuesta.type != LIST_MARKET_TYPE) {
        printError("Error: Invalid response type from island.\n");
        free(mayus_name);
        return;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        free(mayus_name);
        return;
    }
    int num_trozos = partir_datos(respuesta.data, trozos, 5, '&');
    if (num_trozos < 5) {
        printError("Error: malformed market entry.\n");
        free(mayus_name);
        return;
    }
    int total_products = atoi(trozos[1]);
    asprintf(&buffer,"--- %s MARKET ---\n", mayus_name);
    printMessage(buffer);
    free(buffer);
    free(mayus_name);

    asprintf(&buffer,"%s  %d kg  %d gold/kg\n", trozos[2], atoi(trozos[3]), atoi(trozos[4]));
    printMessage(buffer);
    free(buffer);

    for (int i = 1; i < total_products; i++) {
        if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from island.\n");
        return;
        }
        if (respuesta.type != LIST_MARKET_TYPE) {
            printError("Error: Invalid response type from island.\n");
            return;
        }
        if(respuesta.flags == ERROR_FLAG){
            asprintf(&buffer,"%s\n\n",respuesta.data);
            printMessage(buffer);
            free(buffer);
            return;
        }
        num_trozos = partir_datos(respuesta.data, trozos, 5, '&');
        if (num_trozos < 5) {
            printError("Error: malformed market entry.\n");
            return;
        }
        if(strcmp(trozos[2],"MAP") == 0){
            printMessage("Map 50 gold \n");
        }else{
            asprintf(&buffer,"%s  %d kg  %d gold/kg\n", trozos[2], atoi(trozos[3]), atoi(trozos[4]));
            printMessage(buffer);
            free(buffer);
        }
        
    }
    printMessage("\n");
}
int checkErrors(Odysseus *odysseus, char *island_name) {
    if (island_name == NULL) {
        printError("Error: Island name is NULL.\n");
        return -1;
    }
    if (odysseus->viaje_actual.encargo_actual.id == 0) {
        printError("Error: You must accept a voyage before sailing.\n");
        return -1;
    }
    if (odysseus->viaje_actual.ubicacion == ITHACA && strcmp(island_name, "Aeaea") != 0) {
        char *buffer;
        asprintf(&buffer, "We can't sail to %s from here...\n", island_name);
        printError(buffer);
        free(buffer);
        return -1;
    }
    if (odysseus->viaje_actual.isla_actual != NULL && (canIgoToIsland(&odysseus->mapa, island_name, odysseus->viaje_actual.isla_actual)  != 0 || strcmp(island_name, odysseus->viaje_actual.isla_actual) == 0)) {
        char *buffer;
        asprintf(&buffer, "We can't sail to %s from here...\n\n", island_name);
        printError(buffer);
        free(buffer);
        return -1;
    }
    return 0;
}
void comprobarLlegada(Odysseus *odysseus){
    char *buffer;
    if (odysseus->viaje_actual.encargo_actual.destino == NULL) {
        return;
    }
    if (strcmp(odysseus->viaje_actual.encargo_actual.destino, odysseus->viaje_actual.isla_actual) == 0) {
         asprintf(&buffer, "You have reached %s, the destination of your voyage.\n\n",
                 odysseus->viaje_actual.encargo_actual.destino);
        printMessage(buffer);
        free(buffer);
    }
}

int  sendPetitionToIsland(Odysseus *odysseus, char *island_name,char *ip, int port) {
    char *buffer;
    Trama respuesta;

    
    if (sendResponse(odysseus->viaje_actual.fd,ARRIVE_TYPE, REQUEST_FLAG,odysseus->name) < 0) {
        return -1;
    }
    if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        return -1;
    }
    if (respuesta.type != ARRIVE_TYPE) {
        return -1;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        return -1;
    }
    asprintf(&buffer, "Connected to %s.\n", island_name);
    printMessage(buffer);
    free(buffer);
    addIsland(&odysseus->mapa, island_name, ip, port, NULL);
    if(strcmp(respuesta.data,"DOCKED") == 0){
        odysseus->viaje_actual.ubicacion = EN_PUERTO;
        free(odysseus->viaje_actual.isla_actual);
        odysseus->viaje_actual.isla_actual = malloc(strlen(island_name) + 1);
        if (odysseus->viaje_actual.isla_actual == NULL) {
            printError("Error allocating memory for island name\n");
            return -1;
        }
        asprintf(&buffer,"%s has granted access to the port.\n\n", island_name);
        printMessage(buffer);
        free(buffer);
        strcpy(odysseus->viaje_actual.isla_actual, island_name);
        comprobarLlegada(odysseus);
    }else if(strcmp(respuesta.data,"WAIT") == 0){
        odysseus->viaje_actual.ubicacion = ESPERANDO;
        alarm(5);
        free(odysseus->viaje_actual.isla_actual);
        odysseus->viaje_actual.isla_actual = malloc(strlen(island_name) + 1);
        if (odysseus->viaje_actual.isla_actual == NULL) {
            printError("Error allocating memory for island name\n");
            return -1;
        }
        strcpy(odysseus->viaje_actual.isla_actual, island_name);
        printMessage("The harbor is full. We drop anchor and wait.\n");
        
    }
    return 0;
}


void DoSail(Odysseus *odysseus,char *island_name){
    char *buffer;
    Trama respuesta;
    char tipo;
    char *ip;
    int puerto;
    int fd_nuevo;

    if (checkErrors(odysseus, island_name) != 0) {
        return;
    }
    if (strcasecmp(island_name, "Ithaca") == 0) {
        ip= odysseus->itaca_ip;
        puerto = odysseus->itaca_port;
    }else  if (odysseus->viaje_actual.ubicacion == ITHACA) {
        ip = odysseus->ip_start;
        puerto = odysseus->port_start;
    }else {
        IslandVisited *destino = getIsland(&odysseus->mapa, island_name);
        if (destino == NULL) {
            printError("Error: Could not find island in the map.\n");
            return; 
        }
        ip = destino->island_ip;
        puerto = destino->island_port;
    }
    fd_nuevo = createClientConnection(puerto, ip);
    if (fd_nuevo == -1) {
        asprintf(&buffer, "ERROR: Could not connect to %s.\n\n", island_name);
        printError(buffer);
        free(buffer);
        return;
    }
    if (odysseus->viaje_actual.fd != -1) {
         tipo = (odysseus->viaje_actual.ubicacion == ITHACA) ? DISCONNECT_TYPE : LEAVE_TYPE;
        if(sendResponse(odysseus->viaje_actual.fd, tipo, REQUEST_FLAG, "") < 0) {
            printError("Error: Could not send sail request.\n");
            close(fd_nuevo);
            return;
        }
        if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
            printError("Error: Could not receive response from island.\n");
            close(fd_nuevo);
            return;
        }
        if(respuesta.flags == ERROR_FLAG){
            asprintf(&buffer,"%s\n\n",respuesta.data);
            printMessage(buffer);
            free(buffer);
            close(fd_nuevo);
            return;
        }
        asprintf(&buffer, "Leaving %s...\n", (odysseus->viaje_actual.ubicacion == ITHACA) ? "Ithaca" : odysseus->viaje_actual.isla_actual);
        printMessage(buffer);
        free(buffer);
        close(odysseus->viaje_actual.fd);
    }

    
    odysseus->viaje_actual.fd = fd_nuevo;
    odysseus->viaje_actual.ubicacion = MAR;

    asprintf(&buffer, "Sailing to %s...\n\n", island_name);
    printMessage(buffer);
    free(buffer);

    if (sendPetitionToIsland(odysseus, island_name, ip, puerto) != 0) {
        printError("ERROR: Could not send petition to island.\n");
        return;
    }
}

void DoBuyMap(Odysseus *odysseus){
    char *buffer;
    Trama respuesta;
    char *trozos[7];
    int num_trozos = 0;

    if (odysseus->viaje_actual.ubicacion != EN_PUERTO) {
        printMessage("You must be docked at an island to buy a map.\n\n");
        return;
    }

    if(odysseus->money < 50) {
        printMessage("You don't have enough gold to buy a map.\n\n");
        return;
    }
    if(sendResponse(odysseus->viaje_actual.fd, BUY_MAP_TYPE, REQUEST_FLAG, "1") < 0) {
        printError("Error: Could not send buy map request to island.\n");
        return;
    }
    if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from island.\n");
        return;
    }
    if(respuesta.type != BUY_MAP_TYPE) {
        printError("Error: Invalid response type from island.\n");
        return;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        return;
    }
    num_trozos = partir_datos(respuesta.data, trozos, 7, '&');
    if (num_trozos < 7) {
        printError("Error: malformed map entry.\n");
        return;
    }
    int total_islands = atoi(trozos[3]);
    addIsland(&odysseus->mapa, trozos[4], trozos[5], atoi(trozos[6]), odysseus->viaje_actual.isla_actual);
    for(int i = 1; i < total_islands; i++){
        if (recibir_trama(odysseus->viaje_actual.fd, &respuesta) < 0) {
        printError("Error: Could not receive response from island.\n");
        return;
        }
        if(respuesta.type != BUY_MAP_TYPE) {
            printError("Error: Invalid response type from island.\n");
            return;
        }
        if(respuesta.flags == ERROR_FLAG){
            asprintf(&buffer,"%s\n\n",respuesta.data);
            printMessage(buffer);
            free(buffer);
            return;
        }
        num_trozos = partir_datos(respuesta.data, trozos, 7, '&');
        if (num_trozos < 7) {
            printError("Error: malformed map entry.\n");
            return;
        }
        addIsland(&odysseus->mapa, trozos[4], trozos[5], atoi(trozos[6]), odysseus->viaje_actual.isla_actual);
    }
    odysseus->money -= 50;
    asprintf(&buffer,"Bought the map of %s for 50 gold.\n", odysseus->viaje_actual.isla_actual);
    printMessage(buffer);
    free(buffer);
    printMessage("New routes have been charted.\n\n");
}
void DoShowMap(Mapa *mapa){
    SPHRAGIS_Island *lista;
    int r;
    if (mapa->count == 0) {
        printMessage("The map is empty.\n\n");
        return;
    }
    lista = malloc(mapa->count * sizeof(SPHRAGIS_Island));
    if (lista == NULL) {
        printError("Error allocating memory for island list\n");
        return;
    }
    for (int i = 0; i < mapa->count; i++){
        lista[i].name = mapa->islands[i].island_name;
        lista[i].known_islands = mapa->islands[i].connections;
        lista[i].known_island_count = mapa->islands[i].connection_count;
    }
    r = SPHRAGIS_display_map(lista, mapa->count);
    free(lista);
    if (r != SPHRAGIS_OK) {
        printError("Error: could not display the map.\n");
    }
}


void DoBuy(Odysseus *odysseus,char *product_name,int quantity){
    char *buffer;
    Trama respuesta;
    char *trozos[4];
    int num_trozos = 0;
    int cost=0;

    if (odysseus->viaje_actual.ubicacion != EN_PUERTO) {
        printMessage("You must be docked at an island to buy a product.\n\n");
        return;
    }
    asprintf(&buffer, "%s&%d", product_name, quantity);
    if(sendResponse(odysseus->viaje_actual.fd,BUY_TYPE,REQUEST_FLAG,buffer) < 0){
        printError("Error: Could not send buy request to island.\n");
        free(buffer);
        return;
    }
    free(buffer);
    if(recibir_trama(odysseus->viaje_actual.fd,&respuesta)<0){
        printError("Error: Could not receive response from island.\n");
        return;
    }
    if(respuesta.type != BUY_TYPE){
        printError("Error: Invalid response type from island.\n");
        return;
    }
    if(respuesta.flags == ERROR_FLAG){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        return;
    }
    num_trozos = partir_datos(respuesta.data,trozos,4,'&');
    if(num_trozos < 4){
        printError("Error: malformed buy response.\n");
        return;
    }

    cost = atoi(trozos[3]);
    if (odysseus->money < cost) {
        printMessage("You do not have enough gold to buy this product.\n\n");
        return;
    }

    odysseus->money -= cost;
    if(addFood(odysseus, product_name, quantity) != 0){
        printError("Error: Could not add food to cargo.\n");
        return;
    }
    asprintf(&buffer,"Bought %d kg of %s for %d gold.\n\n",quantity,product_name,cost);
    printMessage(buffer);
    free(buffer);
}

void DoSell(Odysseus *odysseus,char *product_name,int quantity){
    char *buffer;
    Trama respuesta;
    int num_trozos = 0;
    char *trozos[4];
    int product_quantity=0;

    if (odysseus->viaje_actual.ubicacion != EN_PUERTO) {
        printMessage("You must be docked at an island to sell a product.\n\n");
        return;
    }

    product_quantity=totalFoodOfThisType(odysseus,product_name);
    if(product_quantity<quantity){
        printMessage("You do not have enough of this product");
        return;
    }
    asprintf(&buffer, "%s&%d", product_name, quantity);
    if(sendResponse(odysseus->viaje_actual.fd,SELL_TYPE,REQUEST_FLAG,buffer) < 0){
        printError("Error: Could not send sell request to island.\n");
        free(buffer);
        return;
    }
    free(buffer);
    if(recibir_trama(odysseus->viaje_actual.fd,&respuesta)<0){
        printError("Error: Could not receive response from island.\n");
        return;
    }
    if(respuesta.type != SELL_TYPE){
        printError("Error: Invalid response type from island.\n");
        return;
    }
    if(respuesta.flags == ERROR_FLAG ){
        asprintf(&buffer,"%s\n\n",respuesta.data);
        printMessage(buffer);
        free(buffer);
        return;
    }
    num_trozos=partir_datos(respuesta.data,trozos,4,'&');
    if(num_trozos < 4){
        printError("Error: malformed buy response.\n");
        return;
    }
    odysseus->money += atoi(trozos[3]);
    if(removeFood(odysseus, product_name, quantity) != 0){
    printError("Error: Could not remove food from cargo.\n");
    return;
    }   

    asprintf(&buffer,"Sold %d kg of %s for %d gold.\n",quantity,product_name,atoi(trozos[3]));
    printMessage(buffer);
    free(buffer);
}



