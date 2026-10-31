#ifndef ITHACA_ACTIONS_H
#define ITHACA_ACTIONS_H

#include "ithaca_config.h"
#include "trama.h"
#include <sys/select.h>

void detectTrama(int fd, Trama *trama, Ithaca *ithaca, char **names, fd_set *current, int *max_fd);

#endif