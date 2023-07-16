CC = gcc
CCFLAGS = -Wall -fdiagnostics-color=always -Wextra -Wconversion -pedantic

BUILD = build
SRC = src
TEST= test
BACKEND = $(SRC)/backend
UTILS = $(SRC)/utils
DS = $(UTILS)/ds
SRCS = $(SRC)/main.c $(BACKEND)/server.c $(BACKEND)/http.c $(DS)/dict.c $(DS)/queue.c $(UTILS)/utils.c

all: debug

.PHONY: debug
debug: $(srcs)
	$(CC) $(SRCS) -o $(BUILD)/main $(CCFLAGS) -g

.PHONY: build
build: $(srcs)
	$(CC) $(SRCS) -o $(BUILD)/main_release $(CCFLAGS) -O2

.PHONY: clean
clean:
	rm -r build/*
	rm -r test/build/*

