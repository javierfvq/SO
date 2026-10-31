#ifndef IO_H
#define IO_H


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

void printMessage   (const char *message);
void printError     (const char *error);

int  atoi_personalizado(char *string);
char *leer_respuesta();
void to_lower_case (char *cadena);
void to_upper_case (char *cadena);

#endif 