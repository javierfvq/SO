#ifndef TERMINAL_H
#define TERMINAL_H

#include "IO.h"
#include "trama.h"
#include "odysseus_config.h"
#include "Connection.h"
#include "OdysseusActions.h"
#include "Commandos.h"



#define CORRECT 0
#define INCORRECT -1



void DetectCommandType(char *command, Odysseus *odysseus);


#endif