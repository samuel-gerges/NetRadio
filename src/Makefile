CC=gcc
CFLAGS=-Wall -g -pthread
EXEC=diff client client2 gestio

all: $(EXEC)

diff: diffuseur.c
	$(CC) $(CFLAGS) -o diff diffuseur.c linked_list.c aux.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c linked_list.c aux.c

client2: client2.c
	$(CC) $(CFLAGS) -o client2 client2.c linked_list.c aux.c

gestio: gestionnaire.c
	$(CC) $(CFLAGS) -o gestio gestionnaire.c linked_list.c aux.c

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

