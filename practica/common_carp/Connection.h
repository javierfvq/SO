#ifndef CONNECTION_H
#define CONNECTION_H

#include "IO.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX_CONNECTIONS 10 

int createServerConnection (int port, char *ip);
int createClientConnection (int port, char *ip);

#endif