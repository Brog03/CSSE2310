#ifndef FILEIO_H
#define FILEIO_H

#define STDOUT_PERM (S_IWUSR | S_IRUSR)
#define F_FD_SIZE 32
#define F_STDIN_ENC (long)0xFFFFFFFF
#define F_STDOUT_ENC (long)0xFFFFFFFF00000000

long init_iofp(char* newStdin, char* newStdout, char* currentFile);
int extract_iofp(long fps, int fp);
void redirect_iofp(int newStdinFp, int newStdoutFp, int childNum, int numChild);
void free_fps(char* newStdin, char* newStdout);

#endif
