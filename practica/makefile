CC = gcc #Pongo el cpompilador a usar

CFLAGS = -Wall -Wextra -Icommon_carp -Iithaca_carp -Iisland_carp -Iodysseus_carp -Icola
COMPARTIDO = common_carp/IO.o common_carp/Files.o \
             common_carp/trama.o common_carp/Connection.o



#Reglas de compilación
all: odysseus ithaca island


OBJS_ITHACA   = ithaca_carp/Ithaca.o ithaca_carp/ithaca_config.o ithaca_carp/ithaca_actions.o
OBJS_ISLAND   = island_carp/Island.o island_carp/island_config.o \
                island_carp/island_actions.o island_carp/market.o cola/queue.o
OBJS_ODYSSEUS = odysseus_carp/Odysseus.o odysseus_carp/odysseus_config.o \
                odysseus_carp/terminal.o odysseus_carp/OdysseusActions.o \
                odysseus_carp/map.o

#Reglas donde digo como se enlazalan los objetos para crear el ejecutable


odysseus: $(OBJS_ODYSSEUS) island_carp/sphragis.o $(COMPARTIDO)
	$(CC) $(CFLAGS) $^ -o $@
	
ithaca: $(OBJS_ITHACA) $(COMPARTIDO)
	$(CC) $(CFLAGS) $^ -o $@

island: $(OBJS_ISLAND) island_carp/sphragis.o $(COMPARTIDO)
	$(CC) $(CFLAGS) $^ -o $@


#Regla generica que he visto que ponen en todos los makefiles, que dice como compilar un archivo .c a un archivo .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
#Para limpiar los archivos .o y el ejecutable y dejar la carpeta limpia
clean:
	rm -f $(OBJS_ITHACA) $(OBJS_ISLAND) $(OBJS_ODYSSEUS) $(COMPARTIDO) \
	      odysseus ithaca island
#Por si hay algun archivo que se llama clean o all
.PHONY: all clean