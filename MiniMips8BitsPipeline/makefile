CC = gcc
CFLAGS = -Wall -g -Iinclude

SRC_FILES = src/main.c src/minimips.c src/controle.c src/decodificador.c src/memoria.c src/multiplexadores.c src/step.c src/pipeline.c src/memoriaDados.c src/interface.c
OBJ_FILES = main.o minimips.o controle.o decodificador.o memoria.o multiplexadores.o step.o pipeline.o memoriaDados.o interface.o

all: exec

exec: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o exec $(OBJ_FILES) -lm -lcurses -lpanel

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o exec log_run.txt

