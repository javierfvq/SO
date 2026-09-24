FICHEROS DE PRUEBA - FASE 1 (The Nostos System)
================================================

--- CASOS NORMALES (deben cargar bien) ---
./odysseus odysseus.dat               Polyphemus, 250 gold, 2 alimentos (Barley 40, DriedFigs 60)
./odysseus odysseus_telemachus.dat    Telemachus, 500 gold, 3 alimentos
./odysseus odysseus_sincomida.dat     Nestor, 100 gold, 0 alimentos (food no debe reventar)
./ithaca ithaca.dat voyages.dat       6 voyages loaded
./ithaca ithaca.dat voyages_vacio.dat 0 voyages loaded
./island aeaea.dat stock_aeaea.db     capacity 2 ships, 3 rutas en fichero, 8 productos
./island scheria.dat stock_scheria.db capacity 2 ships, 2 rutas en fichero, 5 productos
./island aeolia.dat stock_vacio.db    capacity 1 ship, 0 productos
(ismarus, ogygia, thrinacia .dat tambien son validos; el mapa es bidireccional)

OJO: el numero de rutas "loaded" es el que devuelve SPHRAGIS tras filtrar,
no el del fichero. Depende del mapa interno de la libreria.

--- CASOS DE ERROR (deben avisar y salir sin fugas) ---
odysseus_incompleto.dat      falta dinero y comida
odysseus_faltacomida.dat     dice 3 alimentos pero solo hay 2
ithaca_incompleto.dat        falta IP y puerto
voyages_incompleto.dat       la ultima linea no tiene reward
island_incompleto.dat        solo nombre y carpeta
island_rutaincompleta.dat    la ultima ruta no tiene puerto
island_nombreinvalido.dat    la isla se llama "Ithaca" -> SPHRAGIS_ERROR_INVALID_ISLAND

--- CASOS LIMITE (mirad que pasa) ---
island_sinrutas.dat          seccion ROUTES vacia -> 0 rutas, known_islands = NULL
island_rutasfalsas.dat       incluye "Troy", que no es isla valida -> SPHRAGIS la rechaza o da error
island_lineavacia.dat        linea en blanco al final del fichero
island_crlf.dat              saltos de linea de Windows (\r\n)
stock_truncado.db            1 registro completo + 50 bytes sueltos

--- COMPROBAR MEMORIA ---
valgrind --leak-check=full ./island aeaea.dat stock_aeaea.db   (y pulsar CTRL+C)
Debe acabar con "All heap blocks were freed" o sin "definitely lost".

--- GENERAR MAS stock.db ---
python3 gen_stock.py    (editad las listas del final del script)
