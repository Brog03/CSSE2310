#ifndef ERROR_CLIENT_H
#define ERROR_CLIENT_H

#include <stdio.h>
#include <stdlib.h>

#define ERR3_USAGE 3
#define ERR3_USAGE_MSG                                                         \
    "Usage: uqimageclient portnum [--in infile] [--flip dirn |"                \
    " --scale width height | --rotate degrees] [--output outfilename]\n"

#define ERR1_OPEN_FILE_READ 1
#define ERR1_OPEN_FILE_READ_MSG                                                \
    "uqimageclient: unable to open file \"%s\" for reading\n"

#define ERR18_OPEN_FILE_WRITE 18
#define ERR18_OPEN_FILE_WRITE_MSG                                              \
    "uqimageclient: unable to open file \"%s\" for writing\n"

#define ERR2_CANNOT_CONNECT 2
#define ERR2_CANNOT_CONNECT_MSG                                                \
    "uqimageclient: unable to establish connection to port \"%s\"\n"

#define ERR16_FILE_EMPTY 16
#define ERR16_FILE_EMPTY_MSG "uqimageclient: input image is empty\n"

#define ERR9_CANNOT_WRITE 9
#define ERR9_CANNOT_WRITE_MSG "uqimageclient: unable to write output\n"

#define ERR20_CONNECTION_CLOSED 20
#define ERR20_CONNECTION_CLOSED_MSG "uqimageclient: server connection closed\n"

#define ERR14_BAD_REQUEST 14
#define ERR14_BAD_REQUEST_MSG "%.*s"

void exit_err3_usage(void);
void exit_err1_open_file_read(char* fileName);
void exit_err18_open_file_write(char* fileName);
void exit_err2_cannot_connect(char* portNum);
void exit_err16_file_empty(void);
void exit_err9_cannot_write(void);
void exit_err20_connection_closed(void);
void exit_err14_bad_request(void);

#endif
