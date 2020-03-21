# Note: you need the GEM library and includes accessible to
# Turbo C for this to work

CC=tcc
CFLAGS=-ms -DPCGEM -DNEEDGSX 
CLIBS=stcgem.lib 
TARGET=worm.app

clean:
	del *.obj
	del *.app

player.obj: player.c player.h field.h
	$(CC) $(CFLAGS) -c player.c

field.obj: field.c player.h field.h util.h
	$(CC) $(CFLAGS) -c field.c

all: worm.c player.h field.h wormpc.h util.h player.obj field.obj
	$(CC) $(CFLAGS) -e$(TARGET) worm.c player.obj field.obj $(CLIBS)
	