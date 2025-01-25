CC = gcc
CFLAGS = -Isrc/ -Wno-deprecated-declarations -g #-DYYDEBUG=1
LDFLAGS = -lfl

SRC_DIR = src/
BIN_DIR = bin/
SOURCES = $(SRC_DIR)main.c $(SRC_DIR)ast.c $(SRC_DIR)symtab.c \
          $(SRC_DIR)interpreter.c $(SRC_DIR)sl.yy.c $(SRC_DIR)sl.tab.c

all: parser lexer build

parser: $(SRC_DIR)sl.y
	bison -d -o $(SRC_DIR)sl.tab.c $(SRC_DIR)sl.y

lexer: $(SRC_DIR)sl.l
	flex -o $(SRC_DIR)sl.yy.c $(SRC_DIR)sl.l

build:
	mkdir -p bin
	$(CC) $(SOURCES) -o $(BIN_DIR)sl $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(SRC_DIR)*.tab.* $(SRC_DIR)*.yy.* $(BIN_DIR)sl

.PHONY: all parser lexer build clean