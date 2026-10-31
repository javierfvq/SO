#include "trama.h"


void crearTrama(Trama *trama, unsigned char type,unsigned char flags,const char *datos){
    int i = 0;
    trama->type = type;
    trama->flags = flags;
    int length = strlen(datos);
    if (length > DATA_SIZE) {
        length = DATA_SIZE;
    }
    trama->length = (unsigned char)length;
   
    for (i = 0; i < trama->length; i++) {
        trama->data[i] = datos[i];
    }

    trama->data[i] = '\0'; 
}


void montar_trama(const Trama *trama , char *buffer){
    buffer[0]=trama->type;
    buffer[1]=trama->flags;
    buffer[2]=trama->length;

    for (int i=0;i<trama->length;i++){
        buffer[i+3]=trama->data[i];
    }
    for(int i=trama->length+3;i<TOTAL_TRAMA_SIZE;i++){
        buffer[i]=0;
    }
}

int  desmontar_trama(const char *buffer,Trama *trama){
    trama->type=buffer[0];
    trama->flags=buffer[1];
    int length = (unsigned char)buffer[2];
    if (length > DATA_SIZE) {
         return  TRAMA_INVALIDA;
    }
    trama->length = length;
    for(int i=0;i<trama->length;i++){
        trama->data[i]=buffer[i+3];
    }
    trama->data[trama->length]='\0';
    return TRAMA_OK;
}

int enviar_trama(int fd,const Trama *trama){
    
    char buffer[TOTAL_TRAMA_SIZE];
    montar_trama(trama,buffer);

    int count=0;
    while(count<TOTAL_TRAMA_SIZE){
        int num_escritos = write(fd, buffer + count, TOTAL_TRAMA_SIZE - count);
        if (num_escritos < 0) {
            return TRAMA_ERROR;
        }
        count += num_escritos;
    }
    return TRAMA_OK;
}


int partir_datos( char *datos,char *partes[],int max_trozo,char separador){
    int i = 0;
   
    char separador_str[2];
    separador_str[0] = separador;
    separador_str[1] = '\0';

    char *token=strtok(datos, separador_str);
    while(token!=NULL &&  i < max_trozo){
        partes[i]=token;
        i++;
        token=strtok(NULL,separador_str);
    }
    return i;
}

int recibir_trama(int fd, Trama *trama){
    
    char buffer[TOTAL_TRAMA_SIZE];
    int num_leidos = 0;
    
    while (num_leidos < TOTAL_TRAMA_SIZE) {
        int bytes_read = read(fd, buffer + num_leidos, TOTAL_TRAMA_SIZE - num_leidos);
        if (bytes_read < 0) {
            return TRAMA_ERROR;
        }
        if (bytes_read == 0) {
            return TRAMA_CERRADA;
        }
        num_leidos += bytes_read;
    }
    if (desmontar_trama(buffer, trama) != 0) {
        return TRAMA_INVALIDA;
    }

    return TRAMA_OK;
}

int  sendResponse (int fd, unsigned char type, unsigned char flags, char *datos) {
    Trama respuesta;
    crearTrama(&respuesta, type, flags,datos);
    if (enviar_trama(fd, &respuesta) != TRAMA_OK) {
        return TRAMA_ERROR;
    }
    return TRAMA_OK;
}