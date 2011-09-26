CC = gcc 
CFLAGS = -Wall -W -Wshadow -std=gnu99
TARGETS = traceroute
 
all: $(TARGETS)

traceroute: sockwrap.o icmp.o misc.o read.o
	$(CC) traceroute.c sockwrap.o icmp.o misc.o read.o -o traceroute $(CFLAGS)
    
sockwrap.o: sockwrap.c
	$(CC) -c sockwrap.c -o sockwrap.o $(CFLAGS)
    
icmp.o: icmp.c
	$(CC) -c icmp.c -o icmp.o $(CFLAGS)
    
misc.o: misc.c
	$(CC) -c misc.c -o misc.o $(CFLAGS)
    
read.o: read.c
	$(CC) -c read.c -o read.o $(CFLAGS)

clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f $(TARGETS)
