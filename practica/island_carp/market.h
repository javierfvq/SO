#ifndef MARKET_H
#define MARKET_H

#include "island_config.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "trama.h"

#define PRODUCT_NOT_FOUND -1
#define PRODUCT_FOUND 0


#define MARKET_OK 0
#define BUY_ERROR_NOT_ENOUGH_STOCK -1
#define BUY_ERROR_PRODUCT_NOT_FOUND -2
#define SELL_ERROR_PRODUCT_NOT_FOUND -3
#define SELL_ERROR_NOT_ENOUGH_STOCK -4




int searchProductInMarket(Island_CONFIG *island, const char *product_name);
char *ListProductsInMarket(Island_CONFIG *island, int i);


int BuyProductInMarket(Island_CONFIG *island, const char *product_name, int quantity,int *price);
int SellProductInMarket(Island_CONFIG *island, const char *product_name, int quantity,int *price);
char *buyMapInMarket(Island_CONFIG *island, int i);
int saveStockToBinary(Island_CONFIG *island, const char *filename);



#endif