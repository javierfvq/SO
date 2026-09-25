#include "terminal.h"
#include "IO.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define CORRECT 0
#define INCORRECT -1



void to_lower_case (char *cadena) {
  for(int i = 0; i < (int)strlen(cadena); i++) {
    if (cadena[i] >= 'A' && cadena[i] <= 'Z') {
        cadena[i] = cadena[i] + ('a' - 'A');
    }
  }
}

int  detectIsMissingAField (int commandType) {
    char *segunda_palabra = strtok(NULL, " ");
    char *tercera_palabra = strtok(NULL, " ");
   switch (commandType) {

      case CONNECT:
         if (segunda_palabra == NULL || strcmp(segunda_palabra, "ithaca") != 0 || tercera_palabra == NULL) {
            return INCORRECT;
         }else{
            return CORRECT;
         }
         
      break;

      case LIST:
         if (segunda_palabra == NULL ||( strcmp(segunda_palabra, "voyages") != 0 & strcmp(segunda_palabra, "market") != 0) || tercera_palabra == NULL) {
            return INCORRECT;
         } else {
            return CORRECT;
         }
      break;

      case ACCEPT:
         if (segunda_palabra == NULL || atoi_personalizado (segunda_palabra) == -1) {
            return INCORRECT;
         }else{
            return CORRECT;
         }
      break;

      case SAIL:
         if (segunda_palabra == NULL){
            return INCORRECT;
         } else{
            return CORRECT;
         }
      break;

      case MAP:
          if (segunda_palabra == NULL) {
              return CORRECT;
          }else{
              return INCORRECT;
          }
      break;
      
      case BUY:
          if (segunda_palabra == NULL || tercera_palabra == NULL || atoi_personalizado(tercera_palabra) == -1) {
              return INCORRECT;
          }else{
              return CORRECT;
          }
      break;

      case SELL:
          if (segunda_palabra == NULL || tercera_palabra == NULL || atoi_personalizado(tercera_palabra) == -1) {
              return INCORRECT;
          }else{
              return CORRECT;
          }
      break;
      
      case STATUS:
          if (segunda_palabra == NULL) {
            return CORRECT;
          } else {
            return INCORRECT;
          }
      break;

      case DELIVER:
          if (segunda_palabra == NULL) {
            return CORRECT;
          } else {
            return INCORRECT;
          }
      break;

      case CLAIM:
          if (segunda_palabra == NULL) {
            return CORRECT;
          } else {
            return INCORRECT;
          }
      break; 
    }
}

void DoAction(int commandType,char *command){
  int correct=0;
  correct = detectIsMissingAField(commandType);
    
    switch (commandType) {
        case CONNECT:
        if (correct == -1) {
            printError("Usage: CONNECT ITHACA\n");
        } else {
            printMessage("Command OK\n");
        }  
            break;
        case LIST:
            if (correct == -1) {
            printError("Usage: LIST VOYAGES\n" || "Usage: LIST MARKET\n");
        } else {
            printMessage("Command OK\n");
        } 
          break;
        case ACCEPT:
            if (correct == -1) {
                printError("Usage: ACCEPT <ID>\n");
            } else {
                printMessage("Command OK\n");
            } 
            break;
        case SAIL:
            if (correct == -1) {
                printError("Usage: SAIL <ISLAND>\n");
            } else {
                printMessage("Command OK\n");
            }
            break;
        case MAP:
            if (correct == -1) {
                printError("Usage: MAP\n");
            } else {
                printMessage("Command OK\n");
            }
            
            break;
        case BUY:
            if (correct == -1) {
                printError("Usage: BUY <PRODUCT> <AMOUNT>\n");
            } else {
                printMessage("Command OK\n");
            }
            break;
        case SELL:
            if (correct == -1) {
                printError("Usage: SELL <PRODUCT> <AMOUNT>\n");
            } else {
                printMessage("Command OK\n");
            }
            break;
        case STATUS:
            if (correct == -1) {
                printError("Usage: STATUS\n");
            } else {
                printMessage("Command OK\n");
            }
            break;
        case DELIVER:
            if (correct == -1) {
                printError("Usage: DELIVER\n");
            } else {
                printMessage("Command OK\n");
            }
            break;
        case CLAIM:
            if (correct == -1) {
                printError("Usage: CLAIM\n");
            } else {
                printMessage("Command OK\n");
            } 
            break;
        default:
            printError("Unknown command\n");
            break;
    }

    free(command);

}

void DetectCommandType(char *command){
    to_lower_case(command);
    char *primera_palabra = strtok(command," ");
    int commandType = UNKNOWN_COMMAND;

    if (primera_palabra == NULL) {
        commandType = UNKNOWN_COMMAND;
    } else if (strcmp(primera_palabra, "connect") == 0) {
        commandType = CONNECT;
    }else if (strcmp(primera_palabra, "list") == 0) {
        commandType = LIST;
    }else if (strcmp(primera_palabra, "accept") == 0) {
         commandType = ACCEPT;
    }else if (strcmp(primera_palabra, "sail") == 0) {
        commandType = SAIL;
    }else if (strcmp(primera_palabra, "map") == 0) {
          commandType = MAP;
    }else if (strcmp(primera_palabra, "buy") == 0) {
          commandType = BUY;
    }else if (strcmp(primera_palabra, "sell") == 0){
          commandType = SELL;
    }else if (strcmp(primera_palabra, "status") == 0) {
          commandType = STATUS;
    }else if (strcmp(primera_palabra, "deliver") == 0){
          commandType = DELIVER;
    }else if (strcmp(primera_palabra, "claim") == 0){
          commandType = CLAIM;
    }else{
        commandType = UNKNOWN_COMMAND;
    }

    DoAction(commandType,command);  
}




