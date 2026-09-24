#include "IO.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void printMessage (char *message) {
    char *buffer;

    if (asprintf(&buffer, "%s", message) == -1) {
        return;
    } else {
        write(STDOUT_FILENO, buffer, strlen(buffer));
        free(buffer);
    }

}

void printError (char *error) {
    char *buffer;

    if (asprintf(&buffer, "%s", error) == -1) {
        return;
    } else {
        write(STDERR_FILENO, buffer, strlen(buffer));
        free(buffer);
    }

}

char *leer_respuesta() {
    
    char *buffer = malloc(1 * sizeof(char));
    if (!buffer) {
        return NULL; 
    }
    buffer[0] = '\0';

    int count = 0;
    char letra;
    int n; 

    while ((n = read(STDIN_FILENO, &letra, 1)) > 0) {
        if (letra == '\n') {
            break; 
        }

        
        char *buffer2 = realloc(buffer, (count + 1) * sizeof(char));
        if (!buffer2) {
            free(buffer);
            return NULL;
        }
        buffer = buffer2; 

        buffer[count++] = letra;
        buffer[count] = '\0'; 
    }

   
    if (n < 0 && count == 0) {
        free(buffer);
        return NULL;
    }

    return buffer;
}