CC = gcc
CFLAGS = -Wall -g -std=c99
#List of dependencies (.h)
DEPS =
#List of object files (.o)
OBJ = elevator.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
make: $(OBJ)
	$(CC) -o elevator $^ $(CFLAGS)

clean:
	rm *.o
	rm elevator