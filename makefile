P=sign_language
OBJECTS=
CFLAGS=`pkg-config --cflags gtk+-3.0` --std=gnu11 -g -Wall -O3
LDLIBS=`pkg-config --libs gtk+-3.0`
CC=gcc

$(P): $(OBJECTS)
