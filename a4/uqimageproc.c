#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "error_server.h"
#include "server.h"

#define MAX_NUMBER_ARGS 4
#define MAX_CONNECTIONS 10000

typedef struct {
    char* portNum;
    int maxConn;
} RequestArgs;

void validate_params(RequestArgs* params);
bool isdigit_str(char* input);
RequestArgs proccess_argv(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    // Proccess command line arguments
    RequestArgs params = proccess_argv(argc, argv);
    // See if the given argumnets are legall
    validate_params(&params);
    ListenSocket serverSocket = init_server(params.portNum);
    if (serverSocket.socket == -1) {
        exit_err19_cannot_listen(params.portNum);
    }

    process_connections(serverSocket.socket, params.maxConn);

    return 0;
}

// proccess_argv()
//
// Proccesses the command line arguments and stores them in a RequestArgs
// to be later validated
RequestArgs proccess_argv(int argc, char* argv[])
{
    // First argument is the executable, skip over it
    argc--;
    argv++;

    RequestArgs params = {NULL, 0};

    if (argc <= MAX_NUMBER_ARGS) {
        // Provided number of argumnets is less than 4
        for (int i = 0; i < argc; i++) {
            if (i != (argc - 1) && strcmp(argv[i], "--listenon") == 0
                    && !params.portNum && strlen(argv[i + 1])) {
                // --listenon has been provided, next argument isn't empty
                params.portNum = argv[i + 1];
                i++;
            } else if (i != (argc - 1) && strcmp(argv[i], "--maxconn") == 0
                    && !params.maxConn && strlen(argv[i + 1])
                    && isdigit_str(argv[i + 1])) {
                // --maxconn has been provided, next argument is a number
                params.maxConn = atoi(argv[i + 1]);
                i++;
            } else {
                // Argumnet is not legall
                exit_err8_usage();
            }
        }
    } else {
        // More than 4 argumnets have been provided
        exit_err8_usage();
    }

    return params;
}

// validate_params()
//
// Checks to make sure the given parameter:
//      --maxconn value is >= 0 or <= 10000
void validate_params(RequestArgs* params)
{
    if (params->maxConn > MAX_CONNECTIONS || params->maxConn < 0) {
        // --maxconn is either less than 0 or greater than 10000
        exit_err8_usage();
    }

    if (!params->portNum) {
        params->portNum = "0";
    }
}
