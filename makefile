CC = gcc
CCFLAGS = -Wall -fdiagnostics-color=always -pthread
src_dir = src
build_dir = build
srcs = $(src_dir)/main.c $(src_dir)/server.c $(src_dir)/utils.c $(src_dir)/http.c

all: debug

debug: $(srcs)
	$(CC) $(CCFLAGS) -g $(srcs) -o $(build_dir)/main

build: $(srcs)
	$(CC) $(CCFLAGS) $(srcs) -o $(build_dir)/main

clean: 
	rm -r build/*

