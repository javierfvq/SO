#include "terminal.h"
#include "IO.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


void to_lower_case (char *cadena) {
  for(int i = 0; i < strlen(cadena); i++) {
    if (cadena[i] >= 'A' && cadena[i] <= 'Z') {
        cadena[i] = cadena[i] + ('a' - 'A');
    }
  }
}