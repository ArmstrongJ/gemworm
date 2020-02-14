CC=/usr/local/bin/gcc
CFLAGS=-DSTGEM -DDEBUG
CLIBS=-lgem
TARGET=worm.app

clean:
	rm -f *.o
	rm -f *.app

player.o: player.c player.h field.h
	$(CC) $(CFLAGS) -c player.c

field.o: field.c player.h field.h
	$(CC) $(CFLAGS) -c field.c

worm.o: worm.c player.h field.h keybd.h
	$(CC) $(CFLAGS) -c worm.c

all: worm.o player.h field.h keybd.h wormst.h player.o field.o
	$(CC) $(CFLAGS) -o $(TARGET) worm.o player.o field.o $(CLIBS)
	