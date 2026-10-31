#include "market.h"


int searchProductInMarket(Island_CONFIG *island, const char *product_name) {
    for (int i = 0; i < island->product_count; i++) {
        if (strcmp(island->products[i].name, product_name) == 0) {
            return i; 
        }
    }
    return PRODUCT_NOT_FOUND;
}
char *ListProductsInMarket(Island_CONFIG *island, int i) {
    char *resultado = NULL;
    int total = island->product_count + 1;

    if (i == island->product_count) {
        if (asprintf(&resultado, "%d&%d&MAP&1&50", total, total) == -1) {
            return NULL;
        }
        return resultado;
    }

    if (asprintf(&resultado, "%d&%d&%s&%d&%d", i + 1, total, island->products[i].name,
                 island->products[i].amount, island->products[i].price) == -1) {
        return NULL;
    }
    return resultado;
}

int BuyProductInMarket(Island_CONFIG *island, const char *product_name, int quantity,int *price){
    int index = searchProductInMarket(island,product_name);
    if (index == PRODUCT_NOT_FOUND) {
        return BUY_ERROR_PRODUCT_NOT_FOUND;
    }
    if (island->products[index].amount < quantity) {
        return BUY_ERROR_NOT_ENOUGH_STOCK;
    }
    island->products[index].amount -= quantity;
    *price = island->products[index].price * quantity;
    saveStockToBinary(island, island->stock_path);
    return MARKET_OK;
}

int SellProductInMarket(Island_CONFIG *island, const char *product_name, int quantity,int *price){
    int index = searchProductInMarket(island,product_name);
    if (index == PRODUCT_NOT_FOUND) {
        return SELL_ERROR_PRODUCT_NOT_FOUND;
    }
    island->products[index].amount += quantity;
    *price = island->products[index].price * quantity;
    saveStockToBinary(island, island->stock_path);
    return MARKET_OK;
}



char *buyMapInMarket(Island_CONFIG *island, int i) {
    char *resultado = NULL;

    if (asprintf(&resultado, "OK&50&%d&%d&%s&%s&%d", i + 1, island->counter,
                 island->routes[i].name, island->routes[i].ip, island->routes[i].port) == -1) {
        return NULL;
    }
    return resultado;
}


int saveStockToBinary(Island_CONFIG *island, const char *filename){
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printError("Error opening binary file for writing\n");
        return -1;
    }
    for (int i = 0; i < island->product_count; i++) {
        int written = write(fd, &island->products[i], sizeof(Stock));
        if (written != sizeof(Stock)) {
            printError("Error writing to binary file\n");
            close(fd);
            return -1;  
        }
    }
    close(fd);
    return 0;
}

