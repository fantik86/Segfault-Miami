all:
	gcc -g main.c items.c player.c vectors.c -lSDL2 -lSDL2_image -o main
	./main
