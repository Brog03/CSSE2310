#ifndef ERROR_SERVER_H
#define ERROR_SERVER_H

#include <stdio.h>
#include <stdlib.h>

#define ERR8_USAGE 8
#define ERR8_USAGE_MSG "Usage: uqimageproc [--maxconn n] [--listenon port]\n"

#define ERR19_CANNOT_LISTEN 19
#define ERR19_CANNOT_LISTEN_MSG                                                \
    "uqimageproc: cannot listen on given port \"%s\"\n"

void exit_err8_usage(void);
void exit_err19_cannot_listen(char* portNum);

#endif
