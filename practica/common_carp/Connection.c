#include "Connection.h"


int createServerConnection (int port, char *ip){
    int fd_socket;
    int opcion = 1;
    struct sockaddr_in server_addr;

    if ((fd_socket = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        printError("ERROR: Could not create socket\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port =(uint16_t) htons(port);
    if(inet_pton(AF_INET, ip, &server_addr.sin_addr) != 1) {
        printError("ERROR: Invalid IP address\n");
        close(fd_socket);
        return -1;
    }
    setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion));

    if(bind(fd_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        printError("ERROR: Could not bind socket\n");
        close(fd_socket);
        return -1;
    }
    if(listen(fd_socket,MAX_CONNECTIONS) != 0) {
        printError("ERROR: Could not listen on socket\n");
        close(fd_socket);
        return -1;
    }
    return fd_socket;  
}


int createClientConnection (int port,char *ip){
    int fd_socket;
    struct sockaddr_in server_addr;

    fd_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_socket < 0) {
        printError("ERROR: Could not create socket\n");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = (uint16_t) htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) != 1) {
        printError("ERROR: Invalid IP address\n");
        close(fd_socket);
        return -1;
    }
    if (connect(fd_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printError("ERROR: Could not connect to the server\n");
        close(fd_socket);
        return -1;
    }
    return fd_socket;
}