
#include "Files.h"
#include <unistd.h>
#include <stdlib.h>

char *parsear(int fd, char separador) { 
    char caracter;
    char *buffer = NULL; 
    int i = 0; 

    while(read(fd, &caracter, 1) > 0) { 
        buffer = realloc(buffer, (i + 1)*sizeof(char)); 
        if(caracter == '\n' || caracter == separador ){ 
            break;  
        }
        
        buffer[i] = caracter;
        i++;
    } 

   
    if(buffer == NULL){
        return NULL;
    }

    buffer = realloc(buffer, (i + 1)*sizeof(char)); 
    buffer[i] = '\0';

    return buffer;
}

