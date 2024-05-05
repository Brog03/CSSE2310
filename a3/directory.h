#include "inc.h"

#ifndef DIRECTORY_H
#define DIRECTORY_H

#define MAX_CMD_LENGTH 300

struct pArgv {
    char path[MAX_CMD_LENGTH];
    char** fileList;
    int numFiles;

    bool stats;
    bool allfiles;
    bool parallel;
};

CommandPipeline* proccess_argv(int argc, char* argv[], struct pArgv* newArgv);
int filter(const struct dirent* dir);
int read_dir(struct pArgv* newArgv);

char* add_path_to_file_name(char* fileName, char* path);
char* substitute_file_name(char** cmdM, char* cmdS, char* fileName);
char* init_cmd_single(char* cmdS, int fileNameLen);

#endif
