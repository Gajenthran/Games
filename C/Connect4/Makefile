CC=gcc
CFLAGS=-O2 `sdl2-config --cflags`
LINK=-O2 `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
SRC=game.c display.c main.c grid.c sdl_driver.c menu.c player.c
OBJ=$(SRC:.c=.o)

%.o:	%.c
	$(CC) -c $(CFLAGS) $< 

game: 	$(OBJ)
	$(CC) $(LINK) $(OBJ) -o $@

clean:
	rm *.o game
