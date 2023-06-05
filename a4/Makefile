CC=gcc
CFLAGS= -Wall -pedantic -std=gnu99 -g -I/local/courses/csse2310/include
LDFLAGS= -L/local/courses/csse2310/lib -lcsse2310a3 -lcsse2310a4
SERVFLAGS= -lcrypt -lpthread
.PHONY: all
.DEAFAULT_GOAL := all

all: crackclient crackserver
crackclient: crackclient.c crackclient.h cracka4.h 
	$(CC) $(CFLAGS) $(LDFLAGS) crackclient.c -o crackclient
crackserver: crackserver.c crackserver.h cracka4.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(SERVFLAGS) crackserver.c -o crackserver
