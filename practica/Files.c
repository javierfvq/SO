
#include "Files.h"
#include <unistd.h>
#include <stdlib.h>
#include "IO.h"

char *parsear(int fd, char separador) { 
    char caracter;
    char *buffer = NULL; 
    int i = 0; 

    while(read(fd, &caracter, 1) > 0) { 
        if (caracter == '\r') {
            continue; 
        }
        char *temp = realloc(buffer, (i + 1) * sizeof(char));
        if (temp == NULL) {
            printError("Error alocating memory\n");  
            free(buffer); 
            return NULL;
        }
        buffer = temp;
        if(caracter == '\n' || caracter == separador ){ 
            break;  
        }
        
        buffer[i] = caracter;
        i++;
    } 

   
    if(buffer == NULL){
        return NULL;
    }

    char *temp2= realloc(buffer, (i + 1) * sizeof(char));
    if (temp2 == NULL) {
        printError("Error alocating memory\n");  
        free(buffer); 
        return NULL;
    }
    buffer = temp2;
    buffer[i] = '\0';

    return buffer;
}

