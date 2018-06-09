CC = gcc -Wall -pedantic-errors
OBJ = client.o
OBJ2 = server.o

client: $(OBJ)
	$(CC) -o client.out $(OBJ)

client.o: client.c
	$(CC) -c client.c 

server: $(OBJ2)
	$(CC) -o server.out $(OBJ2)

server.o: server.c
	$(CC) -c server.c

clean:
	rm -f *.o *.gch

new:
	rm *.out
