#include "error_client.h"

// exit_err3_usage()
//
// Exits the program if incorrect command line arguments given
void exit_err3_usage(void)
{
    fprintf(stderr, ERR3_USAGE_MSG);
    exit(ERR3_USAGE);
}

// exit_err1_open_file_read()
//
// Exits the program if the --in file specified is unable to be opened
void exit_err1_open_file_read(char* fileName)
{
    fprintf(stderr, ERR1_OPEN_FILE_READ_MSG, fileName);
    exit(ERR1_OPEN_FILE_READ);
}

// exit_err18_open_file_write()
//
// Exits the program if the --out file specified is unable to be opened

void exit_err18_open_file_write(char* fileName)
{
    fprintf(stderr, ERR18_OPEN_FILE_WRITE_MSG, fileName);
    exit(ERR18_OPEN_FILE_WRITE);
}

// exit_err2_cannot_connect()
//
// Exits the program if it cannot cannot to the given portNum
void exit_err2_cannot_connect(char* portNum)
{
    fprintf(stderr, ERR2_CANNOT_CONNECT_MSG, portNum);
    exit(ERR2_CANNOT_CONNECT);
}

// exit_err16_file_empty()
//
// Exits the program if the given image file is empty
void exit_err16_file_empty(void)
{
    fprintf(stderr, ERR16_FILE_EMPTY_MSG);
    exit(ERR16_FILE_EMPTY);
}

// exit_err9_cannot_write()
//
// Exits the program if it cannot write to the STDOUT
void exit_err9_cannot_write(void)
{
    fprintf(stderr, ERR9_CANNOT_WRITE_MSG);
    exit(ERR9_CANNOT_WRITE);
}

// exit_err20_connection_closed()
//
// Exits the program if the connection to the serever is closed
void exit_err20_connection_closed(void)
{
    fprintf(stderr, ERR20_CONNECTION_CLOSED_MSG);
    exit(ERR20_CONNECTION_CLOSED);
}

// exit_err14_bad_request()
//
// Exits the program if the request the server was sent was not allowed
void exit_err14_bad_request(void)
{
    exit(ERR14_BAD_REQUEST);
}
