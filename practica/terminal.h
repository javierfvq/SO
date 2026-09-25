#ifndef TERMINAL_H
#define TERMINAL_H



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

void DetectCommandType(char *command);


#endif