CFLAGS=-Wall -Wextra -pedantic -std=gnu99 -L/local/courses/csse2310/lib -lcsse2310a3
LIBRARIES= -I/local/courses/csse2310/include
CC=gcc

FILE_NAME = uqfindexec
C_FILES = uqfindexec.c directory.c pipes.c stats.c fileio.c
O_FILES = uqfindexec.o directory.o pipes.o stats.o fileio.o
H_FILES = directory.h error.h pipes.h inc.h stats.h fileio.h

all: $(FILE_NAME)

$(FILE_NAME): $(O_FILES) 
	$(CC) $(CFLAGS) $(LIBRARIES) $(O_FILES) -o $@ 

$(O_FILES): $(C_FILES) 
	$(CC) $(CFLAGS) $(LIBRARIES) $(C_FILES) -c


clean: 
	rm -f $(FILE_NAME) *.o

debug: CFLAGS += -g3
debug: all
