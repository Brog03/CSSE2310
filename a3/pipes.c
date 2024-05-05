#include "pipes.h"
#include "error.h"

// Inits the pipe array
int*** init_pipe_array(int numPipes, int numFiles)
{
    // If there is only 1 command, there is no need for pipes
    if (numPipes == 0) {
        return NULL;
    }

    // Allocates each child and 1D array of size two integer pointers
    // for each file
    int*** pipes = malloc(sizeof(int**) * numFiles);

    for (int i = 0; i < numFiles; i++) {
        pipes[i] = malloc(sizeof(int*) * numPipes);
        for (int j = 0; j < numPipes; j++) {
            pipes[i][j] = malloc(sizeof(int) * 2);
            pipe(pipes[i][j]);
        }
    }

    return pipes;
}

// Closes all the pipes
void close_pipes(int*** pipes, int data[DP_SIZE])
{
    int numPipes = data[DP_NUMPIPES];
    int childNum = data[DP_CHILDNUM];
    int numFiles = data[DP_NUMFILES];
    int fileNum = data[DP_FILENUM];

    if (pipes == NULL) {
        return;
    }
    // If we are in the parent, close all pipes
    if (childNum < 0) {
        for (int i = 0; i < numFiles; i++) {
            for (int j = 0; j < numPipes; j++) {
                close(pipes[i][j][RDFD]);
                close(pipes[i][j][WRFD]);
            }
        }

    } else {
        // If the are in the child, redirect STDIN, STDOUT
        // to nececssary pipe
        int** currentPipe = pipes[fileNum];
        if (childNum == 0) {
            dup2(currentPipe[0][WRFD], STDOUT_FILENO);

        } else if (childNum == numPipes) {
            dup2(currentPipe[childNum - 1][RDFD], STDIN_FILENO);

        } else if (numPipes != 1) {
            dup2(currentPipe[childNum - 1][RDFD], STDIN_FILENO);

            dup2(currentPipe[childNum][WRFD], STDOUT_FILENO);
        }

        // Change data so the child can cxall this function acting like
        // the parent to close all pipes
        data[DP_CHILDNUM] = -1;
        data[DP_FILENUM] = -1;
        close_pipes(pipes, data);
    }
}

// Frees all the pipes allocated memory
void free_pipes(int*** pipes, const int data[DP_SIZE])
{
    int numPipes = data[DP_NUMPIPES];
    int childNum = data[DP_CHILDNUM];
    int numFiles = data[DP_NUMFILES];

    if (pipes == NULL) {
        return;
    }

    // If we are in the child
    if (childNum < 0) {
        if (numFiles > 1) {
            for (int i = 0; i < numFiles; i++) {
                for (int j = 0; j < numPipes; j++) {
                    free(pipes[i][j]);
                }
                free(pipes[i]);
            }
            free(pipes);

        } else {
            for (int i = 0; i < numPipes; i++) {
                free(pipes[0][i]);
            }

            free(pipes[0]);
            free(pipes);
        }
    }
}

// Executes the command given to the child
void execute_command(char** cmd, char* fileName)
{
    execvp(cmd[0], cmd);
    fprintf(stderr, ERR_EXE, cmd[0], fileName);
    exit(ERR_EXE_FAIL);
}
