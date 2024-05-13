#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "error.h"

#define DATA_ROTATE 1
#define DATA_SCALE 2
#define DATA_FLIP 3
#define DATA_NULL 0

#define RET_SCALE_X(num) ((num) & (int)0xFFFF0000) >> 16
#define RET_SCALE_Y(num) ((num) & (int)0x0000FFFF)
#define STO_SCALE_X(num) ((num) << 16)
#define STO_SCALE_Y(num) (num)

#define MAX_SCALE_RES (int)(10000)
#define MIN_SCALE_RES (int)(1)
#define MAX_ROTATE_ANGLE (int)(359)
#define MIN_ROTATE_ANGLE (int)(-359)

typedef struct {
    int data;
    int dataType;
    char* portNum;
    char* inFile;
    char* outFile;
} requestArgs;

bool isdigit_str(char* input);
requestArgs proccess_argv(int argc, char* argv[]);
bool validate_params(requestArgs params);

int main(int argc, char* argv[])
{
    requestArgs params = proccess_argv(argc, argv);
    if (!validate_params(params)) {
        exit_invalid_argv();
    }

    return 0;
}

requestArgs proccess_argv(int argc, char* argv[])
{
    requestArgs params = {DATA_NULL, 0, NULL, NULL, NULL};

    argc--;
    argv++;
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--rotate") == 0 && i < (argc - 1)
                && !params.dataType && isdigit_str(argv[i + 1])) {
            // --rotate has been supplied and the next argument is a number
            params.data = atoi(argv[i + 1]);
            params.dataType = DATA_ROTATE;
            i++;
        } else if (strcmp(argv[i], "--scale") == 0
                // --scale has been supplied, next two arguments are numbers
                && !params.dataType && (i < (argc - 2))
                && (isdigit_str(argv[i + 1]) && isdigit_str(argv[i + 2]))
                && (strlen(argv[i + 1]) && strlen(argv[i + 2]))) {

            params.data = STO_SCALE_X(atoi(argv[i + 1]));
            // Store the new x dimension in high byte
            params.data += STO_SCALE_Y(atoi(argv[i + 2]));
            // Store the new y dimension in the low byte
            params.dataType = DATA_SCALE;
            i += 2;
        } else if (strcmp(argv[i], "--flip") == 0 && i < (argc - 1)
                && !params.dataType && strlen(argv[i + 1]) == 1) {
            // --flip has been supplied, next arument is a single char
            params.data = (int)argv[i + 1][0];
            params.dataType = DATA_FLIP;
            i++;
        } else if (strcmp(argv[i], "--in") == 0 && i < (argc - 1)
                && !params.inFile && strlen(argv[i + 1])) {
            // --in file specified and hasn't already been used and isn't empty
            params.inFile = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--out") == 0 && i < (argc - 1)
                && !params.outFile
                && strlen(argv[i + 1])) { // --out file specified and hasn't
                                          // already been used and isnt empty
            params.outFile = argv[i + 1];
            i++;
        } else {
            exit_invalid_argv();
        }
    }
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
bool validate_params(requestArgs params)
{
    bool argsValid = true;

    if (params.dataType == DATA_SCALE
            && ((RET_SCALE_X(params.data) > MAX_SCALE_RES
                        || RET_SCALE_X(params.data) < MIN_ROTATE_ANGLE)
                    || (RET_SCALE_Y(params.data) > MAX_ROTATE_ANGLE
                            || RET_SCALE_Y(params.data) < MIN_ROTATE_ANGLE))) {
        // scale dimensions are not > 0 and not <= 10000
        argsValid = false;
    } else if (params.dataType == DATA_ROTATE
            && (params.data > MAX_ROTATE_ANGLE
                    || params.data < MIN_ROTATE_ANGLE)) {
        // rotate angle is not => -359 and not <= 359
        argsValid = false;
    } else if (params.dataType == DATA_FLIP && params.data != (int)'h'
            && params.data != (int)'v') {
        // flip is not h or v
        argsValid = false;
    }

    return argsValid;
}
