CC = gcc
CFLAGS = -Wall -Werror
SOURCES = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SOURCES))

executable: $(OBJ)
	$(CC) -o $@ $^ 

clean : 
	rm executable *.o


