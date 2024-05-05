#ifndef ERROR_H
#define ERROR_H

#define ERR_EXE_FAIL 56
#define ERR_SIGINT 20
#define ERR_FILE_FAIL 15

#define ERR12 12
#define ERR12_MSG "uqfindexec: cannot read directory \"%s\"\n"

#define ERR4 4
#define ERR4_MSG                                                               \
    "Usage: uqfindexec [--directory dirname] "                                 \
    "[--recurse] [--allfiles] [--parallel] [--stats] [command]\n"

#define ERR17 17
#define ERR17_MSG "uqfindexec: command is invalid\n"

#define ERR_STDIN "uqfindexec: unable to read \"%s\" when processing \"%s\"\n"
#define ERR_STDOUT                                                             \
    "uqfindexec: unable to open \"%s\" for writing when processing \"%s\"\n"

#define ERR_EXE "uqfindexec: cannot execute \"%s\" when processing \"%s\"\n"

#endif
