#ifndef STATS_H
#define STATS_H

#define DS_SIZE 4
#define DS_SUCCESS 0
#define DS_MFAIL 1
#define DS_SIGFAIL 2
#define DS_FAIL 3

#define STAT_HEAD "Attempted to process %d files\n"
#define STAT_N2 " - processing finished successfully for %d files\n"
#define STAT_N3 " - processing may have failed for %d files\n"
#define STAT_N4 " - processing terminated by signal for %d files\n"
#define STAT_N5 " - processing failed for %d files\n"

void print_stats(const int* stats);
void proccess_reap(int** stats, int status, int fileNum);
void calculate_stats(int** stats, int numFiles, int numCmds, int* exitStatus);
int** init_stats(int numFiles);
void free_stats(int** stats, int numFiles);

#endif
