#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void exit_invalid_argv(void)
{
    fprintf(stderr, ERR3_MSG);
    exit(ERR3);
}

void exit_open_file_read(char* fileName)
{
    fprintf(stderr, ERR1_MSG, fileName);
    exit(ERR1);
}

void exit_open_file_write(char* fileName)
{
    fprintf(stderr, ERR18_MSG, fileName);
    exit(ERR18);
}

void exit_cannot_connect(char* portNum)
{
    fprintf(stderr, ERR2_MSG, portNum);
    exit(ERR2);
}

void exit_empty_input(void)
{
    fprintf(stderr, ERR16_MSG);
    exit(ERR16);
}

void exit_cannot_write(void)
{
    fprintf(stderr, ERR9_MSG);
    exit(ERR9);
}

void exit_connection_closed(void)
{
    fprintf(stderr, ERR20_MSG);
    exit(ERR20);
}
