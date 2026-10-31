#ifndef ODYSSEUS_ACTIONS_H
#define ODYSSEUS_ACTIONS_H

#include "Files.h"
#include "IO.h"
#include <fcntl.h>
#include "trama.h"
#include "odysseus_config.h"
#include "Connection.h"
#include "map.h"
#include "sphragis.h"
#include "string.h"



void doConnect(Odysseus *odysseus);
void DoStatus(Odysseus *odysseus);
void DoListVoyages(Odysseus *odysseus);
void DoAccept(Odysseus *odysseus, int id_viaje);
void doDisconnect(Odysseus *odysseus);
void DoSail(Odysseus *odysseus, char *island_name);
void DoListMarket(Odysseus *odysseus);
void DoBuyMap(Odysseus *odysseus);
void DoShowMap(Mapa *mapa);
void DoBuy(Odysseus *odysseus,char *product_name,int quantity);
void DoSell(Odysseus *odysseus,char *product_name,int quantity);
void cerrarConexionOdysseus(Odysseus *odysseus);
void comprobarLlegada(Odysseus *odysseus); 


#endif
