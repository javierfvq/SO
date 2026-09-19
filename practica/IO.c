#include "IO.h"
#include <unistd.h>
#include <stdlib.h>


void printMessage (char *message) {
    char *buffer;

    if (asprintf(&buffer, "%s", message) == NULL) {
        return;
    } else {
        write(STDOUT_FILENO, buffer, strlen(buffer));
        free(buffer);
    }

}

void printError (char *error) {
    char *buffer;

    if (asprintf(&buffer, "%s", error) == NULL) {
        return;
    } else {
        write(STDERR_FILENO, buffer, strlen(buffer));
        free(buffer);
    }

}

