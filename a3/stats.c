#include "inc.h"
#include "stats.h"
#include "error.h"

// Alloctae memory for the stats array
int** init_stats(int numFiles)
{
    int** stats = NULL;

    if (numFiles > 0) {
        stats = malloc(sizeof(int*) * numFiles);
        for (int i = 0; i < numFiles; i++) {
            stats[i] = malloc(sizeof(int) * DS_SIZE);
            for (int j = 0; j < DS_SIZE; j++) {
                stats[i][j] = 0;
            }
        }
    }

    return stats;
}

// Calculates how the child died and stores in the stats array
void proccess_reap(int** stats, int status, int fileNum)
{
    if (stats != NULL) {
        if (WIFEXITED(status)) {
            // If child exit normally
            if (!WEXITSTATUS(status)) {
                // Exited with 0
                stats[fileNum][DS_SUCCESS] += 1;
            } else if (WEXITSTATUS(status) == ERR_EXE_FAIL) {
                // Exited with 56 (did not execute command)
                stats[fileNum][DS_FAIL] += 1;
            } else {
                // Some other exit code defined in the command given
                stats[fileNum][DS_MFAIL] += 1;
            }
        } else if (WIFSIGNALED(status)) {
            // Child died by siganl
            stats[fileNum][DS_SIGFAIL] += 1;
        }
    }
}

void calculate_stats(int** stats, int numFiles, int numCmds, int* exitStatus)
{
    if (stats != NULL) {
        int finalStats[DS_SIZE] = {0, 0, 0, 0};
        for (int i = 0; i < numFiles; i++) {
            if (stats[i][DS_SIGFAIL] > 0) {
                // Child died by signal
                finalStats[DS_SIGFAIL] += 1;
            } else if (stats[i][DS_FAIL] > 0) {
                // File 100% failed, hence set error code to 15
                finalStats[DS_FAIL] += 1;
                if (*(exitStatus) == 0) {
                    *(exitStatus) = ERR_FILE_FAIL;
                }
            } else if (stats[i][DS_SUCCESS] == numCmds) {
                // Field successfully proccessed
                finalStats[DS_SUCCESS] += 1;
            } else if (stats[i][DS_MFAIL] < numCmds) {
                // Since it was nothing else, file might have failed
                finalStats[DS_MFAIL] += 1;
            }
        }

        stats[0][DS_SUCCESS] = finalStats[DS_SUCCESS];
        stats[0][DS_MFAIL] = finalStats[DS_MFAIL];
        stats[0][DS_SIGFAIL] = finalStats[DS_SIGFAIL];
        stats[0][DS_FAIL] = finalStats[DS_FAIL];
    }
}

void print_stats(const int* stats)
{
    int n2Val;
    int n3Val;
    int n4Val;
    int n5Val;

    // Prints 0 for all stats if stats pointer is NULL (no files read)
    if (stats != NULL) {
        n2Val = stats[DS_SUCCESS];
        n3Val = stats[DS_MFAIL];
        n4Val = stats[DS_SIGFAIL];
        n5Val = stats[DS_FAIL];
    } else {
        n2Val = 0;
        n3Val = 0;
        n4Val = 0;
        n5Val = 0;
    }

    int total = n2Val + n3Val + n4Val + n5Val;
    fprintf(stderr, STAT_HEAD, total);
    fprintf(stderr, STAT_N2, n2Val);
    fprintf(stderr, STAT_N3, n3Val);
    fprintf(stderr, STAT_N4, n4Val);
    fprintf(stderr, STAT_N5, n5Val);
}

// Frees stats array
void free_stats(int** stats, int numFiles)
{

    if (numFiles > 0) {
        for (int i = 0; i < numFiles; i++) {
            free(stats[i]);
        }

        free(stats);
    }
}
