CC=gcc
CFLAGS=-Wall -Wextra -std=c11
OBJ=main.o pacientes.o fila.o pilha.o triagem.o hashtable.o

todos: simulador

simulador: $(OBJ)
	$(CC) $(CFLAGS) -o simulador $(OBJ)

main.o: main.c pacientes.h fila.h triagem.h hashtable.h pilha.h
	$(CC) $(CFLAGS) -c main.c

pacientes.o: pacientes.c pacientes.h pilha.h
	$(CC) $(CFLAGS) -c pacientes.c

fila.o: fila.c fila.h pacientes.h
	$(CC) $(CFLAGS) -c fila.c

pilha.o: pilha.c pilha.h pacientes.h
	$(CC) $(CFLAGS) -c pilha.c

triagem.o: triagem.c triagem.h
	$(CC) $(CFLAGS) -c triagem.c

hashtable.o: hashtable.c hashtable.h pacientes.h
	$(CC) $(CFLAGS) -c hashtable.c

clean:
	rm -f *.o simulador
