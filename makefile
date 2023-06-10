CC = gcc
CCFLAGS = -Wall -fdiagnostics-color=always
srcs = src/main.c src/server.c src/utils.c

all: debug

debug: $(srcs)
	$(CC) $(CCFLAGS) -g $(srcs) -o build/main

build: $(srcs)
	$(CC) $(CCFLAGS) $(srcs) -o build/main

clean: 
	rm -r build/*

