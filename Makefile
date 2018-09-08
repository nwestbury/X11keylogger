CC=gcc
LIBS=-lX11
FLAGS=-O3 -Wall

all: keyrecorder keyreplayer

keyrecorder: keyrecorder.c
	$(CC) $(FLAGS) keyrecorder.c $(LIBS) -lXi -o keyrecorder

keyreplayer: keyreplayer.c
	$(CC) $(FLAGS) keyreplayer.c $(LIBS) -lXtst -o keyreplayer

clean:
	rm keyrecorder