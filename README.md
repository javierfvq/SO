# The Nostos System - Grupo 4

##  **Requisitos Previos**

1. **Lenguaje de programación**: C.
3. **Entorno**: dos máquinas Linux de la universidad (en nuestro caso **montserrat** y **matagalls**), ya que Ithaca y las islas deben ejecutarse en máquinas distintas.
---

## Compilación

En **las dos máquinas**, desde la raíz del proyecto:

```bash
make clean && make 
```
### Preparación

```bash
mkdir -p Objects aeaea_files scheria_files polyphemus_files telemachus_files
mkdir -p proves/stockwork
cp proves/stock/*.db proves/stockwork/
```
StockWork es la carpeta donde se guardan las copias de los mercados como se nos pedia en el enunciado 

---

## 📝 Argumentos de cada proceso

```
./ithaca    <config.dat> <voyages.dat>
./island    <config.dat> <stock.db>
./odysseus  <config.dat>
```

Cada isla necesita su propio `.db`: Aeaea usa `Aeaea.db` y Scheria usa `Scheria.db`.

---

##  Ejecución

### Donde tienes que abrir cada proceso
- **montserrat** (`172.16.214.21`): Ithaca (puerto 3001) y los barcos.
- **matagalls** (`172.16.214.20`): Aeaea (puerto 3002) y Scheria (puerto 3003).

### Orden de arranque

Si quieres probar la mayoria de funcionalidad hace falta **cinco terminales**, tiene que seguir el siguiente orden de incicio : primero las islas, después Ithaca, y al final los barcos.

**Terminal 1 — matagalls — Aeaea**
```bash
cd ~/SO/PracticaFase1/practica
./island proves/aeaea.dat proves/stockwork/Aeaea.db
```

**Terminal 2 — matagalls — Scheria**
```bash
./island proves/scheria.dat proves/stockwork/Scheria.db
```
**Terminal 3 — montserrat — Ithaca**
```bash
cd ~/SO/PracticaFase1/practica
./ithaca proves/ithaca.dat proves/voyages.dat
```

**Terminal 4 (Polyphemus)**
```bash
./odysseus proves/odysseus.dat  
```
**Terminales 5 (Telemachus)**
```bash
./odysseus proves/telemachus.dat      
```
---

## 📧 Autores

- **Javier Fernández**: [javier.fv@students.salle.url.edu](mailto:javier.fv@students.salle.url.edu)
- **Daniel Barona**: [daniel.barona@students.salle.url.edu](mailto:daniel.barona@students.salle.url.edu)
