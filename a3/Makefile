CC=gcc
CFLAGS= -Wall -pedantic -std=gnu99 -g -I/local/courses/csse2310/include
LDFLAGS= -L/local/courses/csse2310/lib -lcsse2310a3 -lm
.PHONY: all
.DEAFAULT_GOAL := all

all: testuqwordiply
testuqwordiply: testuqwordiply.c testuqwordiply.h 
	$(CC) $(CFLAGS) $(LDFLAGS) testuqwordiply.c -o testuqwordiply
