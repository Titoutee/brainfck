CC=gcc

bf: src/main.c src/lib.c
	$(CC) -o build/bf src/main.c src/lib.c
