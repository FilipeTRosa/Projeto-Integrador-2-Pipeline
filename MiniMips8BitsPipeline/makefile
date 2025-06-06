CC = gcc
CFLAGS = -Wall -g -Iinclude

SRC_FILES = src/minimips.c src/controle.c src/decodificador.c src/memoria.c src/multiplexadores.c src/step.c src/memoriaDados.c
OBJ_FILES = minimips.o controle.o decodificador.o memoria.o multiplexadores.o step.o memoriaDados.o

all: exec

exec: $(OBJ_FILES) main.o
	$(CC) $(CFLAGS) -o exec main.o $(OBJ_FILES) -lm

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

run: exec
	./exec

clean:
	rm -f *.o exec
