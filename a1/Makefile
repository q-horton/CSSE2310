CC=gcc
CFLAGS= -Wall -pedantic -std=gnu99 -g -I/local/courses/csse2310/include
LDFLAGS= -L/local/courses/csse2310/lib -lcsse2310a1
.PHONY: all
.DEAFAULT_GOAL := all

all: uqwordiply
uqwordiply: uqwordiply.c uqwordiply.h 
	$(CC) $(CFLAGS) $(LDFLAGS) uqwordiply.c -o uqwordiply
