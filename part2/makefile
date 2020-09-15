CC = gcc
CFLAGS = -Wall -g -std=c99
#List of dependencies (.h)
DEPS =
#List of object files (.o)
OBJ = my_timer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
make: $(OBJ)
	$(CC) -o my_timer $^ $(CFLAGS)

clean:
	rm *.o
	rm my_timer