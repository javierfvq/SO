#define _GNU_SOURCE
#include "IO.h"

void printMessage (const char *message) {
    char *buffer;

    if (asprintf(&buffer, "%s", message) == -1) {
        return;
    } else {
        if (write(STDOUT_FILENO, buffer, strlen(buffer)) == -1) {
            free(buffer);
            return;
        }
        free(buffer);
    }

}

void printError ( const char *error) {
    char *buffer;

    if (asprintf(&buffer, "%s", error) == -1) {
        return;
    } else {
        if (write(STDERR_FILENO, buffer, strlen(buffer)) == -1) {
            free(buffer);
            return;
        }
        free(buffer);
    }

}

void to_lower_case (char *cadena) {
    if (cadena == NULL) {
        return;
    }
  for(int i = 0; i < (int)strlen(cadena); i++) {
    if (cadena[i] >= 'A' && cadena[i] <= 'Z') {
        cadena[i] = cadena[i] + ('a' - 'A');
    }
  }
}

void to_upper_case (char *cadena) {
    if (cadena == NULL) {
        return;
    }
  for(int i = 0; i < (int)strlen(cadena); i++) {
    if (cadena[i] >= 'a' && cadena[i] <= 'z') {
        cadena[i] = cadena[i] - ('a' - 'A');
    }
  }
}



int atoi_personalizado(char *string){
    int resultado = 0;

    if (string == NULL || *string == '\0') {
        return -1; 
    }
    if (strlen(string)>9) {
        return -1; 
    }
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] < '0' || string[i] > '9') {
            return -1; 
        }
        
        resultado = resultado * 10 + (string[i] - '0');
        
    }

    if (resultado <= 0) {
        return -1; 
    }
    return resultado;
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

        
        char *buffer2 = realloc(buffer, (count + 2) * sizeof(char));
        if (!buffer2) {
            free(buffer);
            return NULL;
        }
        buffer = buffer2; 

        buffer[count++] = letra;
        buffer[count] = '\0'; 
    }

    if (n == 0 && count == 0) {
        free(buffer);
        return NULL;
    }

   
    if (n < 0) {
        if (errno != EINTR) {
           printError("Error reading input\n");
        }
        free(buffer);
        return NULL;
    }
    return buffer;
}

