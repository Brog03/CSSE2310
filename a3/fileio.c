#include "fileio.h"
#include "error.h"
#include "inc.h"

long init_iofp(char* newStdin, char* newStdout, char* currentFile)
{
    int newStdinFp = 0;
    int newStdoutFp = 0;

    // If STDIN redirection is given
    if (newStdin) {
        // Try oening for reading only
        newStdinFp = open(newStdin, O_RDONLY);

        if (newStdinFp <= 0) {
            // If fails, print error message and return -1;
            fprintf(stderr, ERR_STDIN, newStdin, currentFile);
            newStdinFp = -1;
        }
    }

    // If STDOUT redirection is given
    if (newStdout) {
        // Try opening file for writing only, othewise create it
        newStdoutFp
                = open(newStdout, O_WRONLY | O_CREAT | O_TRUNC, STDOUT_PERM);

        if (newStdoutFp <= 0) {
            fprintf(stderr, ERR_STDOUT, newStdout, currentFile);
            newStdoutFp = -1;
        }
    }

    // Returns teh file pointers in one long int
    return ((long)newStdoutFp << F_FD_SIZE) + newStdinFp;
}

// Extracts the STDIN, STDOUT file pointer from the long int
// using a & bitmask. STDIN is stored in the low byte (32 bits)
// STDOUT is stored in the high byte (32 bit)
int extract_iofp(long fps, int fp)
{
    int file = 0;
    if (fp == STDIN_FILENO) {
        file = fps & F_STDIN_ENC;
    } else if (fp == STDOUT_FILENO) {
        file = (fps & F_STDOUT_ENC) >> F_FD_SIZE;
    }

    return file;
}

// Redirects the STDIN, STDOUT files
void redirect_iofp(int newStdinFp, int newStdoutFp, int childNum, int numChild)
{
    // Redirects STDOUT if the newStdinFp was opened correctly
    // and this is the first child
    if (newStdinFp) {
        if (!childNum) {
            dup2(newStdinFp, STDIN_FILENO);
            close(newStdinFp);
        }
    }

    // Redirects STDOUT if newStdoutFp was opened correctly and this is the
    // last child
    if (newStdoutFp) {
        if (childNum == (numChild - 1)) {
            dup2(newStdoutFp, STDOUT_FILENO);
            close(newStdoutFp);
        }
    }
}

void free_fps(char* newStdin, char* newStdout)
{
    if (newStdin) {
        free(newStdin);
    }

    if (newStdout) {
        free(newStdout);
    }
}
