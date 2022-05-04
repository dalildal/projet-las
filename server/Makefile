CFLAGS=-std=c11 -pedantic -Wall -Wvla -Werror -D_POSIX_C_SOURCE -D_DEFAULT_SOURCE

PRG = server
UTILS_PATH = ../

all: $(PRG)

server: server.c $(UTILS_PATH)utils_v1.h $(UTILS_PATH)utils_v1.o
		cc $(CFLAGS) -o server server.c $(UTILS_PATH)utils_v1.o

utils_v1.o: $(UTILS_PATH)utils_v1.c $(UTILS_PATH)utils_v1.h
	$(CC) $(CCFLAGS) -c $(UTILS_PATH)utils_v1.c

clean:
		rm -f *.o
		rm -f $(PRG)
