CFLAGS=-Wall -Wextra -pedantic -std=gnu99
LIBRARIES= -I/local/courses/csse2310/include
CC=gcc

FILE_NAME = uqimageclient
C_FILES = uqimageclient.c error.c
O_FILES = uqimageclient.o error.o
H_FILES = uqimageclient.h error.h

all: $(FILE_NAME)

$(FILE_NAME): $(O_FILES) 
	$(CC) $(CFLAGS) $(LIBRARIES) $(O_FILES) -o $@ 

$(O_FILES): $(C_FILES) 
	$(CC) $(CFLAGS) $(LIBRARIES) $(C_FILES) -c


clean: 
	rm -f $(FILE_NAME) *.o

debug: CFLAGS += -g3
debug: all
