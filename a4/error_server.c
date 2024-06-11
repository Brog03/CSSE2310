#include "error_server.h"

// exit_err8_usgae()
//
// Exits the program if ilegall command line arguments have been provided
void exit_err8_usage(void)
{
    fprintf(stderr, ERR8_USAGE_MSG);
    exit(ERR8_USAGE);
}

// exit_err17_usgae()
//
// Exits the program if unable to listen on the given port number
void exit_err19_cannot_listen(char* portNum)
{
    fprintf(stderr, ERR19_CANNOT_LISTEN_MSG, portNum);
    exit(ERR19_CANNOT_LISTEN);
}
