#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "error_client.h"
#include "connect.h"

#define MAX_SCALE_RES (int)(10000)
#define MIN_SCALE_RES (int)(1)
#define MAX_ROTATE_ANGLE (int)(359)
#define MIN_ROTATE_ANGLE (int)(-359)

#define STDOUT_PERM (S_IWUSR | S_IRUSR)

typedef struct {
    int data;
    int dataType;
    char* portNum;
    char* inFile;
    char* outFile;
} RequestArgs;

bool isdigit_str(char* input);
RequestArgs proccess_argv(int argc, char* argv[]);
bool validate_params(RequestArgs* params);
void redirect_io(char* inFile, char* outFile);

int main(int argc, char* argv[])
{
    int status = 0;
    // Proccess command line arguments
    RequestArgs params = proccess_argv(argc, argv);

    if (!validate_params(&params)) {
        // Incorrect usage of command line arguments
        exit_err3_usage();
    }

    // redirect STDIN, STDOUT
    redirect_io(params.inFile, params.outFile);
    // Try connecting to the server
    ImageHTTP request = connect_to_server(params.portNum);

    if (request.readEnd == NULL || request.writeEnd == NULL) {
        // Cannot connect to the server
        status = ERR2_CANNOT_CONNECT;
        // Try reading image file into buffer
    } else if (!(status = read_image_into_buffer(&request))) {
        // Image was successfully raed into the buffer
        // Send image to server
        send_image_to_server(request, params.data, params.dataType);
        // Handle the response from the server
        status = handle_http_response(request);
        // Close the reading end of the port
        fclose(request.readEnd);
    }

    // Free Memory
    free(request.data);
    freeaddrinfo(request.ai);

    if (status == ERR20_CONNECTION_CLOSED) {
        // Connection was closed - exit program with code 20
        exit_err20_connection_closed();
    } else if (status == ERR9_CANNOT_WRITE) {
        // Couldn't write to file - exit program with code 9
        exit_err9_cannot_write();
    } else if (status == ERR16_FILE_EMPTY) {
        // Image file was empty - exit program with code 16
        exit_err16_file_empty();
    } else if (status == ERR14_BAD_REQUEST) {
        // Serever recieved a bad request - exit program with code 14
        exit_err14_bad_request();
    } else if (status == ERR2_CANNOT_CONNECT) {
        // Couldn't connect to server - exit program with code 2
        exit_err2_cannot_connect((char*)params.portNum);
    }
    return status;
}

// proccess_argv()
//
// Proccesses the command line arguments and stores them in a RequestArgs
// to be later validated
RequestArgs proccess_argv(int argc, char* argv[])
{
    RequestArgs params = {DATA_NULL, 0, NULL, NULL, NULL};

    argc--;
    argv++;
    int i;
    if (argc > 0) {
        for (i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--rotate") == 0 && i < (argc - 1)
                    && !params.dataType && isdigit_str(argv[i + 1])) {
                // --rotate has been supplied, next argument is a number
                params.data = atoi(argv[i + 1]);
                params.dataType = DATA_ROTATE;
                i++;
            } else if (strcmp(argv[i], "--scale") == 0 && (i < (argc - 2))
                    && !params.dataType
                    && (isdigit_str(argv[i + 1]) && isdigit_str(argv[i + 2]))
                    && (strlen(argv[i + 1]) && strlen(argv[i + 2]))) {
                // --scale has been supplied, next two arguments are ints
                params.data = STO_SCALE_X(atoi(argv[i + 1]));
                // Store the new x dimension in high 16 bits
                params.data += STO_SCALE_Y(atoi(argv[i + 2]));
                // Store the new y dimension in the low 16 bits
                params.dataType = DATA_SCALE;
                i += 2;
            } else if (strcmp(argv[i], "--flip") == 0 && i < (argc - 1)
                    && !params.dataType && strlen(argv[i + 1]) == 1) {
                // --flip has been supplied, next argument is a single char
                params.data = (int)argv[i + 1][0];
                params.dataType = DATA_FLIP;
                i++;
            } else if (strcmp(argv[i], "--in") == 0 && i < (argc - 1)
                    && !params.inFile && strlen(argv[i + 1])) {
                // --in file specified, hasn't been used, not empty
                params.inFile = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], "--output") == 0 && i < (argc - 1)
                    && !params.outFile && strlen(argv[i + 1])) {
                // --out file specified, hasn't been used, not empty
                params.outFile = argv[i + 1];
                i++;
            } else {
                exit_err3_usage();
            }
        }
    } else {
        exit_err3_usage();
    }
    params.portNum = argv[0];
    return params;
}

// isdigit_str()
//
// exactly the same as isdigit() except only returns true if the whole string
// is only numbers 0-9
bool isdigit_str(char* input)
{
    bool isNumber = true;

    for (int i = 0; i < (int)strlen(input); i++) {
        if (!isdigit(input[i])) {
            isNumber = false;
            break;
        }
    }

    return isNumber;
}

// validate_params()
//
// Checks to make sure the given parameter:
//      --flip value is either 'h' or 'v'
//      --rotate value is in [-359, 359]
//      --scale values is in (0, 10000]
bool validate_params(RequestArgs* params)
{
    bool argsValid = true;

    if (params->dataType == DATA_SCALE
            && ((RET_SCALE_X(params->data) > MAX_SCALE_RES
                        || RET_SCALE_X(params->data) < MIN_ROTATE_ANGLE)
                    || (RET_SCALE_Y(params->data) > MAX_ROTATE_ANGLE
                            || RET_SCALE_Y(params->data) < MIN_ROTATE_ANGLE))) {
        // scale dimensions are not > 0 and not <= 10000
        argsValid = false;
    } else if (params->dataType == DATA_ROTATE
            && (params->data > MAX_ROTATE_ANGLE
                    || params->data < MIN_ROTATE_ANGLE)) {
        // rotate angle is not => -359 and not <= 359
        argsValid = false;
    } else if (params->dataType == DATA_FLIP && params->data != (int)'h'
            && params->data != (int)'v') {
        // flip is not h or v
        argsValid = false;
    } else if (params->dataType == DATA_NULL) {
        params->dataType = DATA_ROTATE;
        params->data = 0;
    }

    if (!strlen(params->portNum)) {
        argsValid = false;
    }

    return argsValid;
}

// redirect_io()
//
// Redirects STDIN to inFile and STDOUT to outFile
void redirect_io(char* inFile, char* outFile)
{
    if (inFile) {
        // --in has been provided, try opening file for reading
        int newStdinFp = open(inFile, O_RDONLY);

        if (newStdinFp <= 0) {
            // If file is unable to be opened
            exit_err1_open_file_read(inFile);
        } else {
            // If file has successfully been opened for reading, redirect STDIN
            dup2(newStdinFp, STDIN_FILENO);
            close(newStdinFp);
        }
    }

    if (outFile) {
        // --out has been provided, try opening the file for only writing
        // If file doesn't exist, create a new one, otherwise Truncate it
        int newStdoutFp
                = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, STDOUT_PERM);

        if (newStdoutFp <= 0) {
            // File is unable to be opened
            exit_err18_open_file_write(outFile);
        } else {
            // File has successfiully been opened, redirect STDOUT
            dup2(newStdoutFp, STDOUT_FILENO);
            close(newStdoutFp);
        }
    }
}
