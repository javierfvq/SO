#ifndef COMMANDOS_H
#define COMMANDOS_H

enum CommandType{
    CONNECT,
    LIST,
    ACCEPT,
    SAIL,
    MAP,
    BUY,
    SELL,
    STATUS,
    DELIVER,
    CLAIM,
    UNKNOWN_COMMAND
};



typedef struct{
    int tipo;
    char *args[4];
    int num_args;
}Command;

#endif