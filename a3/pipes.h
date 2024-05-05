#include "inc.h"

#ifndef PIPES_H
#define PIPES_H

#define RDFD 0
#define WRFD 1

#define DP_SIZE 4
#define DP_NUMPIPES 0
#define DP_CHILDNUM 1
#define DP_NUMFILES 2
#define DP_FILENUM 3

#define PARENT_PARA_DATA                                                       \
    (const int[4])                                                             \
    {                                                                          \
        numPipes, -1, numFiles, -1                                             \
    }

#define PARENT_NORM_DATA                                                       \
    (const int[4])                                                             \
    {                                                                          \
        numPipes, -1, 1, -1                                                    \
    }

#define CHILD_NORM_DATA                                                        \
    (const int[4])                                                             \
    {                                                                          \
        numPipes, j, 1, 0                                                      \
    }

#define CHILD_PARA_DATA                                                        \
    (const int[4])                                                             \
    {                                                                          \
        numPipes, j, numFiles, i                                               \
    }

int*** init_pipe_array(int numPipes, int numFiles);
void close_pipes(int*** pipes, int data[DP_SIZE]);
void free_pipes(int*** pipes, const int data[DP_SIZE]);
void execute_command(char** cmd, char* fileName);

#endif
