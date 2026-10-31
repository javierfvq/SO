#ifndef TRAMA_H
#define TRAMA_H

#include "IO.h"

#define TOTAL_TRAMA_SIZE 256
#define DATA_SIZE 253

#define CONNECT_TYPE 0X01
#define LIST_VOYAGES_TYPE 0X02
#define ACCEPT_TYPE 0X03
#define CLAIM_TYPE 0x04
#define DISCONNECT_TYPE 0X05 
#define ARRIVE_TYPE 0X10
#define LIST_MARKET_TYPE 0X11
#define BUY_TYPE 0X12
#define SELL_TYPE 0X13
#define BUY_MAP_TYPE 0X14
#define DELIVER_TYPE 0X15
#define LEAVE_TYPE 0X16
#define NACK_TYPE 0X7F


#define RESPONSE_BIT_FLAG 0X80
#define ERROR_BIT_FLAG 0X40
#define BINARY_BIT_FLAG 0X20


#define REQUEST_FLAG 0X00
#define OK_FLAG 0X80
#define ERROR_FLAG 0XC0
#define BINARY_FLAG 0X20


#define TRAMA_OK        0
#define TRAMA_ERROR    -1
#define TRAMA_CERRADA  -2
#define TRAMA_INVALIDA -3



typedef struct{
    unsigned char type;
    unsigned char flags;
    unsigned char length;
    char data[DATA_SIZE+1];
} Trama;

void crearTrama(Trama *trama, unsigned char type, unsigned char flags, const char *datos);
void montar_trama(const Trama *trama, char *buffer);
int  desmontar_trama(const char *buffer, Trama *trama);
int enviar_trama(int fd, const Trama *trama);
int recibir_trama(int fd, Trama *trama);
int partir_datos( char *datos,char *partes[],int max_trozo,char separador);
int sendResponse(int fd, unsigned char type, unsigned char flags, char *datos);

#endif