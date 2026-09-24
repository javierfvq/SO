#!/usr/bin/env python3
# Genera ficheros stock.db con el formato del enunciado:
#   char name[100]; int amount; int price;   -> 108 bytes por registro
import struct, sys

FMT = "<100sii"  # little-endian, igual que en Montserrat (x86_64)

def write_stock(path, products):
    with open(path, "wb") as f:
        for name, amount, price in products:
            f.write(struct.pack(FMT, name.encode(), amount, price))

if __name__ == "__main__":
    write_stock("stock_aeaea.db", [
        ("Barley", 140, 7), ("OliveOil", 110, 12), ("DriedFish", 90, 15),
        ("DriedFigs", 80, 9), ("Wine", 60, 20), ("Cheese", 50, 18),
        ("Honey", 40, 25), ("Bread", 70, 5),
    ])
    write_stock("stock_scheria.db", [
        ("Wine", 100, 18), ("Barley", 200, 6), ("Lamb", 30, 40),
        ("Olives", 120, 8), ("Figs", 75, 10),
    ])
    open("stock_vacio.db", "wb").close()
    # Un registro completo + 50 bytes sueltos (registro cortado)
    write_stock("stock_truncado.db", [("Barley", 140, 7)])
    with open("stock_truncado.db", "ab") as f:
        f.write(struct.pack(FMT, b"Wine", 60, 20)[:50])
