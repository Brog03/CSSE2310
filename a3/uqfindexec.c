#include "inc.h"
#include "pipes.h"
#include "directory.h"
#include "error.h"
#include "stats.h"
#include "fileio.h"

bool sigc = false;

void signal_handle()
{
    sigc = true;
}

int main(int argc, char* argv[])
{
    struct pArgv newArgv = {"", 0, 0, false, false, false};
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    CommandPipeline* pipeLine = proccess_argv(argc, argv, &newArgv);
    act.sa_handler = signal_handle;
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, 0);
    int*** allPipes;
    int newStdinFp = 0;
    int newStdoutFp = 0;
    int numFiles = newArgv.numFiles;
    int numCmds = pipeLine->numCmds;
    int numPipes = numCmds - 1;
    int exitStatus = 0;
    int parentData[DP_SIZE];
    int childData[DP_SIZE];
    int** stats = init_stats(numFiles);
    pid_t childPids[numFiles][numCmds];
    if (newArgv.parallel) {
        allPipes = init_pipe_array(numPipes, numFiles);
        memcpy(parentData, PARENT_PARA_DATA, sizeof(PARENT_PARA_DATA));
    } else {
        memcpy(parentData, PARENT_NORM_DATA, sizeof(PARENT_PARA_DATA));
    }
    int status;
    for (int i = 0; i < numFiles; i++) {
        if (sigc) {
            exitStatus = ERR_SIGINT;
            if (!newArgv.parallel) {
                continue;
            }
        }
        char* currentFile = newArgv.fileList[i];
        if (!newArgv.parallel) {
            allPipes = init_pipe_array(numPipes, 1);
        }
        char* newStdin = substitute_file_name(
                NULL, pipeLine->stdinFileName, currentFile);
        char* newStdout = substitute_file_name(
                NULL, pipeLine->stdoutFileName, currentFile);
        long fps = init_iofp(newStdin, newStdout, currentFile);

        if ((newStdinFp = extract_iofp(fps, STDIN_FILENO)) == -1
                || (newStdoutFp = extract_iofp(fps, STDOUT_FILENO)) == -1) {
            stats[i][DS_FAIL] += 1;
            free_pipes(allPipes, parentData);
            continue;
        }
        for (int j = 0; j < numCmds; j++) {
            pid_t currentPid;
            char** currentCmd = pipeLine->cmdArray[j];
            fflush(stdout);
            if ((currentPid = fork())) {
                childPids[i][j] = currentPid;
            } else {
                if (newArgv.parallel) {
                    memcpy(childData, CHILD_PARA_DATA, sizeof(CHILD_PARA_DATA));
                } else {
                    memcpy(childData, CHILD_NORM_DATA, sizeof(CHILD_NORM_DATA));
                }
                redirect_iofp(newStdinFp, newStdoutFp, j, numCmds);
                close_pipes(allPipes, childData);
                substitute_file_name(currentCmd, NULL, currentFile);
                execute_command(currentCmd, currentFile);
            }
        }
        if (!newArgv.parallel) {
            close_pipes(allPipes, parentData);
            for (int j = 0; j < numCmds; j++) {
                waitpid(childPids[i][j], &status, 0);
                proccess_reap(stats, status, i);
            }
            free_pipes(allPipes, parentData);
        }

        free_fps(newStdin, newStdout);
    }
    if (newArgv.parallel) {
        close_pipes(allPipes, parentData);
        for (int i = 0; i < numFiles; i++) {
            for (int j = 0; j < numCmds; j++) {
                waitpid(childPids[i][j], &status, 0);
                proccess_reap(stats, status, i);
            }
        }
        free_pipes(allPipes, parentData);
    }
    calculate_stats(stats, numFiles, numCmds, &exitStatus);

    if (newArgv.stats) {
        if (stats != NULL) {
            print_stats(stats[0]);
        } else {
            print_stats(NULL);
        }
    }
    free_stats(stats, numFiles);
    free_pipeline(pipeLine);
    return exitStatus;
}
