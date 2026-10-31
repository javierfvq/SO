#include "terminal.h"


static const char *errorStrings[] = {
    "Usage: CONNECT ITHACA\n\n",                
    "Usage: LIST VOYAGES / LIST MARKET\n\n",    
    "Usage: ACCEPT <voyage_id>\n\n",           
    "Usage: SAIL <island>\n\n",                
    "Usage: MAP\n\n",                          
    "Usage: BUY <product> <amount>\n\n",        
    "Usage: SELL <product> <amount>\n\n",      
    "Usage: STATUS\n\n",                        
    "Usage: DELIVER\n\n",                      
    "Usage: CLAIM\n\n",                        
    "Unknown command\n\n", 
};




int  detectIsMissingAField (Command *command_struct) {
   
    switch (command_struct->tipo) {

      case CONNECT:
        if (command_struct->num_args!=1 || command_struct->args[0] == NULL || strcmp(command_struct->args[0], "ithaca") != 0) {
            return INCORRECT;
        } else {
            return CORRECT;
        }
      break;

      case LIST:
        to_lower_case(command_struct->args[0]);
         if (command_struct->args[0] == NULL  || command_struct->args[1] != NULL) {
            return INCORRECT;
         } else if (strcmp(command_struct->args[0], "voyages") == 0) {
            return CORRECT;
         }else if (strcmp(command_struct->args[0], "market") == 0) {
            
            return CORRECT;
         } else {
            return INCORRECT;
         } 
      break;

      case ACCEPT:
         if (command_struct->args[0] == NULL || command_struct->args[1] != NULL || atoi_personalizado(command_struct->args[0]) == -1) {
            return INCORRECT;
         } else {
            return CORRECT;
         }
      break;

      case SAIL:
            if (command_struct->args[0] == NULL || command_struct->args[1] != NULL) {
                return INCORRECT;
            } else {
                return CORRECT;
            }
      break;

      case MAP:
          if (command_struct->args[0] == NULL) {
              return CORRECT;
          }else{
              return INCORRECT;
          }
      break;
      
      case BUY:
          if (command_struct->args[0] == NULL || command_struct->args[1] == NULL || atoi_personalizado(command_struct->args[1]) == -1 || command_struct->args[2] != NULL) {
              return INCORRECT;
          }else{
                return CORRECT;
          }
          
      break;

      case SELL:
          if (command_struct->args[0] == NULL || command_struct->args[1] == NULL || atoi_personalizado(command_struct->args[1]) == -1 || command_struct->args[2] != NULL) {
              return INCORRECT;
          }else{
                return CORRECT;
          }
      break;
      
      case STATUS:
         if (command_struct->args[0] == NULL) {
            return CORRECT;
         } else {
            return INCORRECT;
         }
      break;

      case DELIVER:
          if (command_struct->args[0] == NULL) {
            return CORRECT;
          } else {
            return INCORRECT;
          }
      break;

      case CLAIM:
        if (command_struct->args[0] == NULL) {
            return CORRECT;
        } else {
         return INCORRECT;
        }
        break; 
      default:
          return INCORRECT;
          break;
         
    }
}

void parsear_comando(char *linea, Command *command_struct) {
    char *palabras[5];
    int i = 0;

    i= partir_datos(linea, palabras, 5, ' ');
    
    if (i==0){
        command_struct->tipo = UNKNOWN_COMMAND;
        command_struct->num_args = 0;
        return;
    }
    to_lower_case(palabras[0]);
    if (strcmp(palabras[0], "connect") == 0) {
        command_struct->tipo = CONNECT;
    } else if (strcmp(palabras[0], "list") == 0) {
        command_struct->tipo = LIST;
    } else if (strcmp(palabras[0], "accept") == 0) {
        command_struct->tipo = ACCEPT;
    } else if (strcmp(palabras[0], "sail") == 0) {
        command_struct->tipo = SAIL;
    } else if (strcmp(palabras[0], "map") == 0) {
        command_struct->tipo = MAP;
    } else if (strcmp(palabras[0], "buy") == 0) {
        command_struct->tipo = BUY;
    } else if (strcmp(palabras[0], "sell") == 0) {
        command_struct->tipo = SELL;
    } else if (strcmp(palabras[0], "status") == 0) {
        command_struct->tipo = STATUS;
    } else if (strcmp(palabras[0], "deliver") == 0) {
        command_struct->tipo = DELIVER;
    } else if (strcmp(palabras[0], "claim") == 0) {
        command_struct->tipo = CLAIM;
    } else {
        command_struct->tipo = UNKNOWN_COMMAND;
    }

    for (int j=1; j<i; j++){
        command_struct->args[j-1] = palabras[j];
    }
    command_struct->num_args = i - 1;

    if ((command_struct->tipo == CONNECT || command_struct->tipo == LIST)
        && command_struct->num_args >= 1) {
        to_lower_case(command_struct->args[0]);
    }
}


void DoAction(Command *command_struct, Odysseus *odysseus) {
  int commandType = command_struct->tipo;
  int correct=0;
  correct = detectIsMissingAField(command_struct);

  if (correct == -1){
     printMessage(errorStrings[commandType]);
  }else{
    switch (commandType) {
      case CONNECT:
        doConnect(odysseus);
        break;
      case LIST:
        if (strcmp(command_struct->args[0], "voyages") == 0) {
            DoListVoyages(odysseus);
           
        } else if (strcmp(command_struct->args[0], "market") == 0) {
          DoListMarket(odysseus);
            
        }
        break;
      case ACCEPT:
        DoAccept(odysseus, atoi_personalizado(command_struct->args[0]));
       
        break;
      case SAIL:
        DoSail(odysseus, command_struct->args[0]);
        
        break;
      case MAP:
        DoShowMap(&odysseus->mapa);
        break;
      case BUY:
        if (strcasecmp(command_struct->args[0], "map") == 0) {
            DoBuyMap(odysseus);
        }else{
            DoBuy(odysseus, command_struct->args[0], atoi_personalizado(command_struct->args[1]));
        }
        break;
      case SELL:
        DoSell(odysseus, command_struct->args[0], atoi_personalizado(command_struct->args[1]));
        break;
      case STATUS:
        DoStatus(odysseus);
        break;
      case DELIVER:
        
        break;
      case CLAIM:
       
        break;
      default:
        printError(errorStrings[UNKNOWN_COMMAND]);
    }
  }
}

void DetectCommandType(char *command, Odysseus *odysseus) {
    Command command_struct={0};
    int i=0;

    if (command == NULL || strlen(command) == 0){
        return ;
    }
    while (command[i] == ' ' && command[i] != '\0') {
        i++;
    }
    if (command[i] == '\0') {
        return;
    }

    parsear_comando(command, &command_struct);
    DoAction(&command_struct,odysseus);  
}




