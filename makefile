CC=g++

UNAME_S := $(shell uname -s)

LIB_NAME=icdr
INC_DIR=-Isrc/
OBJ_DIR=obj/Release/
BIN_DIR=bin/Release/
LIBFLAGS=-shared -fPIC

ifeq ($(UNAME_S), Linux)
	CFLAGS=-O3 -std=c++11 -lpthread -Wall -Werror -Wno-unused-but-set-variable -Wno-sign-compare -Wno-unused-value -Wno-unused-result
	LIB_FILENAME = $(LIB_NAME).so
endif
ifeq ($(UNAME_S), Darwin)
	CFLAGS=-O3 -std=c++11 -Wall -Werror
	LIB_FILENAME = $(LIB_NAME).dylib
endif

LIB_PATH=$(LIB_NAME)/$(LIB_NAME)/$(LIB_FILENAME)

ICDR: $(LIB_NAME).o
	$(CC)  -o  $(BIN_DIR)ICDR  $(OBJ_DIR)$(LIB_NAME).o  $(CFLAGS)  -s
	$(CC)  $(CFLAGS)  $(LIBFLAGS)  $(LIB_NAME)_exposed_gnu.cpp  -o  $(LIB_PATH)
	cp $(LIB_PATH) $(BIN_DIR)$(LIB_FILENAME)

$(LIB_NAME).o: $(LIB_NAME)_exposed_gnu.cpp
	mkdir -p -m777 $(BIN_DIR)
	mkdir -p -m777 $(OBJ_DIR)
	mkdir -p -m777 $(BIN_DIR)output
	$(CC)  $(INC_DIR)  $(CFLAGS)  -c  test.cpp  -o  $(OBJ_DIR)$(LIB_NAME).o

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

