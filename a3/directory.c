#include "directory.h"
#include "error.h"

CommandPipeline* proccess_argv(int argc, char* argv[], struct pArgv* newArgv)
{
    int dirLoc = 0;
    // Sets default command to echo the file name
    char cmd[MAX_CMD_LENGTH] = "echo {}";
    CommandPipeline* pipeLine = NULL;
    for (int i = 1; i < argc; i++) {
        // Checks if first two characters of the argument ar --
        // and if that argument has not already been used
        if (strlen(argv[i]) >= 2 && argv[i][0] == '-' && argv[i][1] == '-') {
            // If they are, sets the relevant bool
            if (strcmp(argv[i], "--parallel") == 0 && !newArgv->parallel) {
                newArgv->parallel = true;
            } else if (strcmp(argv[i], "--stats") == 0 && !newArgv->stats) {
                newArgv->stats = true;
            } else if (strcmp(argv[i], "--allfiles") == 0
                    && !newArgv->allfiles) {
                newArgv->allfiles = true;
            } else if (strcmp(argv[i], "--directory") == 0 && !dirLoc
                    && i != argc - 1) {
                strcpy(newArgv->path, argv[i + 1]);
                dirLoc = i + 1;
                // If the argument starts with -- but is not one of the allowed
                // arguments, print usage message
            } else {
                fprintf(stderr, ERR4_MSG);
                exit(ERR4);
            }
            // If the i is the directory location and it has't been used
        } else if (i == dirLoc && dirLoc && strlen(argv[i])) {
            strcpy(newArgv->path, argv[i]);
            // If we get here but its not the last argument, means command is
            // not last
        } else if (i != argc - 1 || !strlen(argv[argc - 1])) {
            fprintf(stderr, ERR4_MSG);
            exit(ERR4);
            // If we get here, it is the las argument in argv, so therefore it
            // must be the command
        } else if (i == argc - 1) {
            strcpy(cmd, argv[i]);
        }
    }
    // read the directory and parse the command to get commands
    newArgv->numFiles = read_dir(newArgv);
    // If the cmd is invalid, print command invalid
    if ((pipeLine = parse_pipeline_string(cmd)) == NULL) {
        fprintf(stderr, ERR17_MSG);
        exit(ERR17);
    }

    return pipeLine;
}

int read_dir(struct pArgv* newArgv)
{
    // Set the sorting method to emulate ls
    setlocale(LC_COLLATE, "en_AU");
    struct dirent** dir;
    // Scan the directory
    int numFiles;

    if (strlen(newArgv->path)) {
        numFiles = scandir(newArgv->path, &dir, filter, alphasort);
    } else {
        numFiles = scandir(".", &dir, filter, alphasort);
    }
    char** fileList;

    int count = 0;

    if (numFiles > 0) {
        // Assume the directory has 1 file
        fileList = malloc(sizeof(char*));
        for (int i = 0; i < numFiles; i++) {
            char* dirName = dir[i]->d_name;
            // Only adds d_name staring with '.' only if --allfiles was added
            if ((newArgv->allfiles && dirName[0] == '.') || dirName[0] != '.') {
                if (count != 0) {
                    // Adds more space for more files if needed
                    fileList = realloc(fileList, sizeof(char*) * (count + 1));
                }

                fileList[count] = add_path_to_file_name(dirName, newArgv->path);
                count++;
            }
        }

        newArgv->fileList = fileList;

    } else if (numFiles != 0) {
        fprintf(stderr, ERR12_MSG, newArgv->path);
        exit(ERR12);
    }

    return count;
}

// Function that scandir uses to only include the files if:
// It is a file or a symbolic link to a file
int filter(const struct dirent* dir)
{
    struct stat slinkStats;
    memset(&slinkStats, 0, sizeof(slinkStats));
    int include = 0;

    // Use the dirent struct to see if its a regular file
    if (dir->d_type == DT_REG) {
        include = 1;
    } else if (dir->d_type == DT_LNK) {
        // If its a symbolic link, see if it points to a regular file
        stat(dir->d_name, &slinkStats);
        if (!S_ISLNK(slinkStats.st_mode)) {
            lstat(dir->d_name, &slinkStats);
            if (!S_ISREG(slinkStats.st_mode)) {
                include = 1;
            }
        }
    }

    return include;
}

char* add_path_to_file_name(char* fileName, char* path)
{
    // Make space for the new file name, including a '\0' and a potential '/'
    int newFileNameLen = strlen(path) + strlen(fileName) + 2;
    char* newFileName = malloc(sizeof(char) * newFileNameLen);
    strcpy(newFileName, path);

    // Check to see if path has a trailing '/'
    //  If not, add one
    if (strlen(path)) {
        if (path[strlen(path) - 1] != '/') {
            strcat(newFileName, "/");
        }
    }
    strcat(newFileName, fileName);

    return newFileName;
}

// If this function is called in the parent, it will be using cmdS (Singular)
// Which will just replace the occurence of {} in the stdout, stdin fileNames
// With copies as the parent must copy this data and not overwrite
char* substitute_file_name(char** cmdM, char* cmdS, char* fileName)
{
    // If multiple commands arent given, i.e a file name, or a sub command
    char* newCmdS = NULL;
    if (!cmdM) {
        if (cmdS != NULL) {
            // Allocate some memory to store enough room for all instances of {}
            // being replaced by fileName and one extra space for a '\0'
            newCmdS = init_cmd_single(cmdS, strlen(fileName));
            // If there are no instances of {}, return some dynamic memory
            if (newCmdS == NULL) {
                newCmdS = malloc(sizeof(char) * (strlen(cmdS) + 1));
                strcpy(newCmdS, cmdS);
            } else {
                char* rCmdS = cmdS;
                int index = 0;
                int rIndex = 0;
                while (cmdS[index]) {
                    // If we hit a "{}"
                    if (cmdS[index] == '{' && cmdS[index + 1] == '}') {
                        // Set the first "{" = to a '\0' so strcpy and strcat
                        // dont copy the "{}"
                        cmdS[index] = '\0';
                        // Coppies everything before that bracket
                        if (!index) {
                            strcpy(newCmdS, rCmdS);
                        } else {
                            strcat(newCmdS, rCmdS);
                        }
                        // Resets cmdS, points rCmdS to char after "}"
                        strcat(newCmdS, fileName);
                        cmdS[index] = '{';
                        rCmdS += (rIndex + 2);
                        index += 1;
                        rIndex = 0;
                    }
                    index++;
                    rIndex++;
                }
                strcat(newCmdS, rCmdS);
            }
        }
    } else if (cmdM) {
        int index = 0;
        // Loop through each string and treat them as a singular command
        while (cmdM[index] != NULL) {
            cmdM[index] = substitute_file_name(NULL, cmdM[index], fileName);
            index++;
        }
    }
    return newCmdS;
}

// Counts the amount "{}" in cmdS
char* init_cmd_single(char* cmdS, int fileNameLen)
{
    int index = 0;
    int delimiterCount = 0;
    int cmdSLen = 0;
    char* newCmdS = NULL;

    while (cmdS[index]) {
        if (cmdS[index] == '{' && cmdS[index + 1] == '}') {
            delimiterCount++;
            index++;
        }

        index++;
    }

    // If the string contains "{}", make enough space to store
    // "{}" replaced by fileName
    if (delimiterCount != 0) {
        // For every fileName, take the length of "{}" (2), then add one
        // extra char for the ending '\0'
        cmdSLen = delimiterCount * (fileNameLen - 2) + strlen(cmdS) + 1;
        newCmdS = malloc(sizeof(char) * cmdSLen);
        memset(newCmdS, 0, sizeof(char) * cmdSLen);
    }

    return newCmdS;
}
