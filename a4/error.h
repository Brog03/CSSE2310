#ifndef ERROR_H
#define ERROR_H

#define ERR3 3
#define ERR3_MSG                                                               \
    "Usage: uqimageclient portnum [--in infile] [--flip dirn |"                \
    "--scale width height | --rotate degrees] [--output outfilename]\n"

#define ERR1 1
#define ERR1_MSG "uqimageclient: unable to open file \"%s\" for reading\n"

#define ERR18 18
#define ERR18_MSG "uqimageclient: unable to open file \"%s\" for writing\n"

#define ERR2 2
#define ERR2_MSG                                                               \
    "uqimageclient: unable to establish connection to port \"%s\"\n"

#define ERR16 16
#define ERR16_MSG "uqimageclient: input image is empty\n"

#define ERR9 9
#define ERR9_MSG "uqimageclient: unable to write output\n"

#define ERR20 20
#define ERR20_MSG "uqimageclient: server connection closed"

void exit_invalid_argv(void);
void exit_open_file_read(char* fileName);
void exit_open_file_write(char* fileName);
void exit_cannot_connect(char* portNum);
void exit_empty_input(void);
void exit_cannot_write(void);
void exit_connection_closed(void);

#endif
