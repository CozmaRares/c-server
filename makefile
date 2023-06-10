CC=gcc

all: debug 

debug: src/main.c
	$(CC) -Wall -fdiagnostics-color=always -g src/main.c -o build/main 

build: src/main.c
	$(CC) -Wall -Werror -fdiagnostics-color=always -g src/main.c -o build/main

clean: 
	rm -r build/*


