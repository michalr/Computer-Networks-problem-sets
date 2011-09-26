CC = gcc 
CFLAGS = -lm -Wall -W -Wshadow -std=gnu99
TARGETS = udp-downloader
 
all: $(TARGETS)

udp-downloader: connection.o misc.o read.o
	$(CC) udp-downloader.c connection.o misc.o read.o -o udp-downloader $(CFLAGS)
    
connection.o: connection.c
	$(CC) -c connection.c -o connection.o $(CFLAGS)
    
misc.o: misc.c
	$(CC) -c misc.c -o misc.o $(CFLAGS)
	
read.o: read.c
	$(CC) -c read.c -o read.o $(CFLAGS)
    
clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f $(TARGETS)
