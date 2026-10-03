CC = gcc #Pongo el cpompilador a usar
CFLAGS = -Wall -Wextra  #Los flags de compilación que solemos usar en clase

COMPARTIDO = IO.o Files.o #Los archivos que se comparten entre los tres ejecutables



#Reglas de compilación
all: odysseus ithaca island

#Reglas donde digo como se enlazalan los objetos para crear el ejecutable

odysseus: Odysseus.o terminal.o $(COMPARTIDO)
	$(CC) $(CFLAGS) $^ -o $@

ithaca: Ithaca.o $(COMPARTIDO)
	$(CC) $(CFLAGS) $^ -o $@
island: Island.o sphragis.o $(COMPARTIDO)
	$(CC) $(CFLAGS) $^ -o $@


#Regla generica que he visto que ponen en todos los makefiles, que dice como compilar un archivo .c a un archivo .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
#Para limpiar los archivos .o y el ejecutable y dejar la carpeta limpia
clean:
	rm -f Odysseus.o terminal.o Ithaca.o Island.o $(COMPARTIDO) odysseus ithaca island

#Por si hay algun archivo que se llamara clean o all
.PHONY: all clean