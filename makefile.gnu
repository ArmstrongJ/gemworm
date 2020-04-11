CC=gcc
CFLAGS=-DSTGEM
CLIBS=-lgem
TARGET=worm.app

clean:
	rm -f player.o field.o scores.o worm.o
	rm -f $(TARGET)

player.o: player.c player.h field.h
	$(CC) $(CFLAGS) -c player.c

field.o: field.c player.h field.h
	$(CC) $(CFLAGS) -c field.c

scores.o: scores.c scores.h 
	$(CC) $(CFLAGS) -c scores.c

worm.o: worm.c player.h field.h keybd.h scores.h
	$(CC) $(CFLAGS) -c worm.c

all: worm.o player.h field.h keybd.h wormst.h player.o field.o scores.o
	$(CC) $(CFLAGS) -o $(TARGET) worm.o player.o field.o scores.o $(CLIBS)
	
