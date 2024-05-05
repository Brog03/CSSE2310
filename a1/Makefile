CC = gcc -I/local/courses/csse2310/include
CFLAGS= -Wall -Wextra -pedantic -std=gnu99 -L/local/courses/csse2310/lib -lcsse2310a1

FILE_NAME = uqunscramble
C_FN = $(FILE_NAME).c
O_FN = $(FILE_NAME).o

all: uqunscramble

uqunscramble: uqunscramble.o
	$(CC) $(CFLAGS) $(O_FN) -o $@ 

uqunscramble.o: $(C_FN)
	$(CC) $(CFLAGS) $(C_FN) -c

clean: 
	rm -f $(FILE_NAME) *.o

clean_test:
	rm -f *.stderr *.stdout

debug: CFLAGS += -g3
debug: all
