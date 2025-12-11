@echo off

set CC=g++.exe

set LIB_NAME=icdr
set INC_DIR=-Isrc\
set OBJ_DIR=obj\Release\
set BIN_DIR=bin\Release\

set CFLAGS=-O3 -std=c++11 -lpthread -Wall -Werror -Wno-unused-but-set-variable -Wno-sign-compare -Wno-unused-value -Wno-unused-result
set LIBFLAGS=-shared
set DLLFLAGS=-Wl,--subsystem,windows


mkdir %BIN_DIR%
mkdir %OBJ_DIR%
mkdir %BIN_DIR%output

%CC%  %INC_DIR%  %CFLAGS%  -c  test.cpp  -o  %OBJ_DIR%%LIB_NAME%.o
%CC%  -o  %BIN_DIR%%LIB_NAME%.exe  %OBJ_DIR%%LIB_NAME%.o  %CFLAGS%  -s

%CC%  %INC_DIR%  %CFLAGS%  -c  icdr_exposed_dll.cpp  -o  %OBJ_DIR%%LIB_NAME%.o
%CC%  %CFLAGS%  -o  %LIB_NAME%\%LIB_NAME%\%LIB_NAME%.dll -s %LIBFLAGS%  %OBJ_DIR%%LIB_NAME%.o %DLLFLAGS%

copy %LIB_NAME%\%LIB_NAME%\%LIB_NAME%.dll %BIN_DIR%%LIB_NAME%.dll
