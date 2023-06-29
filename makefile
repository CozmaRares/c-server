CC = gcc
CCFLAGS = -Wall -fdiagnostics-color=always -pthread

BUILD_DIR = build
SRC_DIR = src
BACKEND = $(SRC_DIR)/backend
UTILS = $(SRC_DIR)/utils
DS = $(UTILS)/ds
SRCS = $(SRC_DIR)/main.c $(BACKEND)/server.c $(BACKEND)/http.c $(DS)/dict.c $(DS)/queue.c  $(UTILS)/utils.c

all: debug

debug: $(srcs)
	$(CC) -g $(SRCS) -o $(BUILD_DIR)/main $(CCFLAGS)

build: $(srcs)
	$(CC) $(SRCS) -o $(BUILD_DIR)/main $(CCFLAGS)

clean:
	rm -r build/*

