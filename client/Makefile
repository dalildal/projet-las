CFLAGS=-std=c11 -pedantic -Wall -Wvla -Werror -D_POSIX_C_SOURCE -D_DEFAULT_SOURCE

PRG = client
UTILS_PATH = ../

all: $(PRG)

client: client.c $(UTILS_PATH)utils_v1.h $(UTILS_PATH)utils_v1.o
		cc $(CFLAGS) -o client client.c $(UTILS_PATH)utils_v1.o

utils_v1.o: $(UTILS_PATH)utils_v1.c $(UTILS_PATH)utils_v1.h
	$(CC) $(CCFLAGS) -c $(UTILS_PATH)utils_v1.c

clean:
		rm -f *.o
		rm -f $(PRG)
