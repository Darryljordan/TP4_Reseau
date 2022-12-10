OBJ1 = client.o
OBJ2 = serveur.o

EXEC = ${OBJ1} client ${OBJ2} serveur
all: ${EXEC}

client.o : client.c client.h
	gcc  -g -c  client.c

serveur.o : serveur.c serveur.h
	gcc  -g -c  serveur.c

client : ${OBJ1}
	gcc ${OBJ1} -o client

serveur : ${OBJ2}
	gcc ${OBJ2} -o serveur

clean :
	rm ${EXEC}
