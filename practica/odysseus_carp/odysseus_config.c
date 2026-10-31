#define _GNU_SOURCE
#include "odysseus_config.h"

int load_odysseus(const char *path, Odysseus *odysseus) {
    
    int fd = open(path,O_RDONLY);

    if (fd<0){
        char *buffer;
        if (asprintf(&buffer, "ERROR: Path does not exist: %s\n", path) == -1) {
            return -1;
        }
        printMessage(buffer);
        free(buffer);
        return -1;
    }else{
            
            char *folder = parsear(fd, ' ');
            char *name = parsear(fd, ' ');
            char *itaca_ip = parsear(fd, ' ');
            char *itaca_port = parsear(fd, ' ');
            char *ip_start = parsear(fd, ' ');
            char *port_start = parsear(fd, ' ');
            char *money = parsear(fd, ' ');
            char *food_count = parsear(fd, '\n');
            
            if (folder == NULL|| itaca_ip == NULL || name == NULL || itaca_port == NULL || ip_start == NULL || port_start == NULL || money == NULL || food_count == NULL) {
                free(folder);
                free(itaca_ip);
                free(name);
                free(itaca_port);
                free(ip_start);
                free(port_start);
                free(money);
                free(food_count);
                printError("ERROR: a field is missing.\n");
                close(fd);
                return -1;
            }else{
                
                odysseus->folder = folder;
                odysseus->name = name;
                odysseus->itaca_ip = itaca_ip;
                odysseus->itaca_port = atoi(itaca_port);
                free(itaca_port);
                odysseus->ip_start = ip_start;
                odysseus->port_start = atoi(port_start);
                free(port_start);
                odysseus->money = atoi(money);
                free(money);
                odysseus->food_count = atoi(food_count);
                free(food_count);
                
                odysseus->food = malloc(odysseus->food_count * sizeof(Food));
                if (odysseus->food == NULL) {
                    odysseus->food_count = 0;
                    printError("Error allocating memory for food\n");
                    close(fd);
                    return -1;
                }


                for (int i = 0; i < odysseus->food_count; i++) {
                    char *food_name = parsear(fd, ' ');
                    char *food_quantity = parsear(fd, '\n');
                    if (food_name == NULL || food_quantity == NULL) {
                        printError("Error: a field is missing\n");
                        free(food_name);
                        free(food_quantity);
                        odysseus->food_count = i;
                        close(fd);
                        return -1;
                    }else{
                        odysseus->food[i].name = food_name;
                        odysseus->food[i].quantity = atoi(food_quantity);
                        free(food_quantity);
                    }
                }
            }
    }
    close(fd);
    return 0;
}

int addFood(Odysseus *odysseus, const char *food_name, int kg) {
        int found = 0;
        for (int i = 0; i < odysseus->food_count; i++) {
            if (strcmp(odysseus->food[i].name, food_name) == 0) {
                odysseus->food[i].quantity += kg;
                found = 1;
                break;
            }
        }
        if (!found) {
            Food *new_food = realloc(odysseus->food, (odysseus->food_count + 1) * sizeof(Food));
            if (new_food == NULL) {
                printError("Error allocating memory for new food item\n");
                return -1;
            }
            odysseus->food = new_food;
            odysseus->food[odysseus->food_count].name = malloc(strlen(food_name) + 1);
            if (odysseus->food[odysseus->food_count].name == NULL) {
                printError("Error allocating memory for food name\n");
                return -1;
            }
            strcpy(odysseus->food[odysseus->food_count].name, food_name);
            odysseus->food[odysseus->food_count].quantity = kg;
            odysseus->food_count++;
        }
        return 0;
}
int removeFood(Odysseus *odysseus,char *food_name,int kg){
        int found = 0;
        for(int i=0;i<odysseus->food_count;i++){
            if (strcmp(odysseus->food[i].name,food_name) == 0){
                found = 1;
                if(odysseus->food[i].quantity < kg){
                    printError("Error: Not enough food to remove\n");
                    return -1;
                }else{
                    odysseus->food[i].quantity -= kg;
                    if(odysseus->food[i].quantity == 0){
                        free(odysseus->food[i].name);
                        for(int j=i;j<odysseus->food_count-1;j++){
                            odysseus->food[j] = odysseus->food[j+1];
                        }
                        odysseus->food_count--;
                    }
                    return 0;
                }
            }
        }
        if (!found){
            printError("Error: Food not found\n");
            return -1;
        }
        return 0;
}
int removeFoodOfThisType(Odysseus *odysseus, const char *food_name) {
    for (int i = 0; i < odysseus->food_count; i++) {
        if (strcmp(odysseus->food[i].name, food_name) == 0) {
            free(odysseus->food[i].name);
            for (int j = i; j < odysseus->food_count - 1; j++) {
                odysseus->food[j] = odysseus->food[j + 1];
            }
            odysseus->food_count--;
            return 0;
        }
    }
    printError("Error: Food not found\n");
    return -1;
}
int totalFoodWeight(Odysseus *odysseus){
    int total_weight=0;
    for(int i=0;i<odysseus->food_count;i++){
        total_weight += odysseus->food[i].quantity;
    }
    return total_weight;
}
int totalFoodOfThisType(Odysseus *odysseus, const char *food_name){
    for(int i=0;i<odysseus->food_count;i++){
        if(strcmp(odysseus->food[i].name, food_name) == 0){
            return odysseus->food[i].quantity;
        }
    }
    return 0;
}

int eatProvisional(Odysseus *odysseus,int kg){
    int total_kg_eaten = 0;
    int i = 0;
    if(totalFoodWeight(odysseus) < kg){
        return -1;
    }
    while (i<odysseus->food_count && total_kg_eaten < kg) {
        if (odysseus->food[i].quantity <= (kg - total_kg_eaten)){
            total_kg_eaten += odysseus->food[i].quantity;
            removeFoodOfThisType(odysseus, odysseus->food[i].name);
        } else {
            odysseus->food[i].quantity -= (kg - total_kg_eaten);
            total_kg_eaten = kg;
        }
    }
    return total_kg_eaten == kg ? 0 : -1;
}
