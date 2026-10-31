#ifndef FILES_H
#define FILES_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "IO.h"
#include <string.h>
#include <time.h>
#include <signal.h>



char *parsear(int fd, char separador);


#endif 

