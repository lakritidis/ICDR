CC=g++

UNAME_S := $(shell uname -s)

LIB_NAME=icdr
INC_DIR=-Isrc/
OBJ_DIR=obj/Release/
BIN_DIR=bin/Release/
LIBFLAGS=-shared -fPIC

ifeq ($(UNAME_S), Linux)
	WARNING_FLAGS=-Wall -Wno-sign-compare
	OPTIMIZATION_FLAGS=-O3 -std=c++11 -flto
	LIB_FILENAME = $(LIB_NAME).so
endif
ifeq ($(UNAME_S), Darwin)
	WARNING_FLAGS=-Wall -Wall -Werror
	OPTIMIZATION_FLAGS=-O3 -std=c++11
	LIB_FILENAME = $(LIB_NAME).dylib
endif

LIB_PATH=$(LIB_NAME)/$(LIB_NAME)/$(LIB_FILENAME)

$(LIB_NAME).so: $(LIB_NAME).o
	$(CC)  -o  $(BIN_DIR)$(LIB_NAME)  $(OBJ_DIR)$(LIB_NAME).o  $(CFLAGS) -s -lpthread
	$(CC)  $(OPTIMIZATION_FLAGS)  $(LIBFLAGS)  $(LIB_NAME)_exposed_gnu.cpp  -o  $(LIB_PATH)
	cp $(LIB_PATH) $(BIN_DIR)$(LIB_FILENAME)

$(LIB_NAME).o: $(LIB_NAME)_exposed_gnu.cpp
	mkdir -p -m777 $(BIN_DIR)
	mkdir -p -m777 $(OBJ_DIR)
	$(CC)   $(WARNING_FLAGS) $(OPTIMIZATION_FLAGS) $(INC_DIR)  -c  test.cpp  -o  $(OBJ_DIR)$(LIB_NAME).o

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

