/***********************************************************************************************************************************
Harness for Testing Using Fork

Sometimes it is useful to use a child process for testing.  This can be to test interaction with another process or to avoid
polluting memory in the main process with something that can't easily be undone.

The general form of the fork harness is:

HARNESS_FORK_BEGIN()
{
    // This block is required and can be repeated up to HARNESS_FORK_CHILD_MAX times.
    //
    // The first parameter is the expected exit code.  If the child block does not have an explicit exit then it will automatically
    // exit on 0.
    //
    // The second paramater specifies whether pipes should be setup between the parent and child processes.  These can be accessed
    // with the HARNESS_FORK_*() macros;
    HARNESS_FORK_CHILD_BEGIN(0, true)
    {
        // Child test code goes here
    }
    HARNESS_FORK_CHILD_END();

    // This block is optional but generally useful
    HARNESS_FORK_PARENT_BEGIN()
    {
        // Parent test code goes here
    }
    HARNESS_FORK_PARENT_END();
}
HARNESS_FORK_END();

If the child process does not explicitly exit in HARNESS_FORK_CHILD_BEGIN/END() then it will exit with 0 at HARNESS_FORK_END().
This harness is not intended for long-lived child processes.

There should not be any code outside the HARNESS_FORK_CHILD_BEGIN/END() and HARNESS_FORK_PARENT_BEGIN/END() blocks.
***********************************************************************************************************************************/
#include <sys/wait.h>
#include <unistd.h>

#include <common/fork.h>

/***********************************************************************************************************************************
Define the max number of child processes allowed
***********************************************************************************************************************************/
#define HARNESS_FORK_CHILD_MAX                                      4

/***********************************************************************************************************************************
Total number of child processes forked
***********************************************************************************************************************************/
#define HARNESS_FORK_PROCESS_TOTAL()                                                                                               \
    HARNESS_FORK_processTotal

/***********************************************************************************************************************************
Return the process index of the child (i.e. the index in the total)
***********************************************************************************************************************************/
#define HARNESS_FORK_PROCESS_IDX()                                                                                                 \
    HARNESS_FORK_processIdx

/***********************************************************************************************************************************
Return the id of the child process, 0 if in the child process
***********************************************************************************************************************************/
#define HARNESS_FORK_PROCESS_ID(processId)                                                                                         \
    HARNESS_FORK_processIdList[processId]

/***********************************************************************************************************************************
Return the pipe for the child process
***********************************************************************************************************************************/
#define HARNESS_FORK_PIPE(processId)                                                                                               \
    HARNESS_FORK_pipe[processId]

/***********************************************************************************************************************************
Is the pipe required for this child process?
***********************************************************************************************************************************/
#define HARNESS_FORK_PIPE_REQUIRED(processId)                                                                                      \
    HARNESS_FORK_pipeRequired[processId]

/***********************************************************************************************************************************
Get read/write pipe handles
***********************************************************************************************************************************/
#define HARNESS_FORK_CHILD_READ_PROCESS(processId)                                                                                 \
    HARNESS_FORK_PIPE(processId)[1][0]

#define HARNESS_FORK_CHILD_READ()                                                                                                  \
    HARNESS_FORK_CHILD_READ_PROCESS(HARNESS_FORK_PROCESS_IDX())

#define HARNESS_FORK_CHILD_WRITE_PROCESS(processId)                                                                                \
    HARNESS_FORK_PIPE(processId)[0][1]

#define HARNESS_FORK_CHILD_WRITE()                                                                                                 \
    HARNESS_FORK_CHILD_WRITE_PROCESS(HARNESS_FORK_PROCESS_IDX())

#define HARNESS_FORK_PARENT_READ_PROCESS(processId)                                                                                \
    HARNESS_FORK_PIPE(processId)[0][0]

#define HARNESS_FORK_PARENT_WRITE_PROCESS(processId)                                                                               \
    HARNESS_FORK_PIPE(processId)[1][1]

/***********************************************************************************************************************************
At the end of the HARNESS_FORK block the parent will wait for the child to exit.  By default an exit code of 0 is expected but that
can be modified when the child begins.
***********************************************************************************************************************************/
#define HARNESS_FORK_CHILD_EXPECTED_EXIT_STATUS(processId)                                                                         \
    HARNESS_FORK_expectedExitStatus[processId]

/***********************************************************************************************************************************
Begin the fork block
***********************************************************************************************************************************/
#define HARNESS_FORK_BEGIN()                                                                                                       \
    do                                                                                                                             \
    {                                                                                                                              \
        unsigned int HARNESS_FORK_PROCESS_TOTAL() = 0;                                                                             \
        pid_t HARNESS_FORK_PROCESS_ID(HARNESS_FORK_CHILD_MAX) = {0};                                                               \
        int HARNESS_FORK_CHILD_EXPECTED_EXIT_STATUS(HARNESS_FORK_CHILD_MAX) = {0};                                                 \
        bool HARNESS_FORK_PIPE_REQUIRED(HARNESS_FORK_CHILD_MAX) = {0};                                                             \
        int HARNESS_FORK_PIPE(HARNESS_FORK_CHILD_MAX)[2][2] = {{{0}}};

/***********************************************************************************************************************************
Create a child process
***********************************************************************************************************************************/
#define HARNESS_FORK_CHILD_BEGIN(expectedExitStatus, pipeRequired)                                                                 \
    do                                                                                                                             \
    {                                                                                                                              \
        HARNESS_FORK_CHILD_EXPECTED_EXIT_STATUS(HARNESS_FORK_PROCESS_TOTAL()) = expectedExitStatus;                                \
                                                                                                                                   \
        if (pipeRequired)                                                                                                          \
        {                                                                                                                          \
            HARNESS_FORK_PIPE_REQUIRED(HARNESS_FORK_PROCESS_TOTAL()) = true;                                                       \
                                                                                                                                   \
            THROW_ON_SYS_ERROR_FMT(                                                                                                \
                pipe(HARNESS_FORK_PIPE(HARNESS_FORK_PROCESS_TOTAL())[0]) == -1, KernelError,                                       \
                "unable to create read pipe for child process %u", HARNESS_FORK_PROCESS_TOTAL());                                  \
            THROW_ON_SYS_ERROR_FMT(                                                                                                \
                pipe(HARNESS_FORK_PIPE(HARNESS_FORK_PROCESS_TOTAL())[1]) == -1, KernelError,                                       \
                "unable to create write pipe for child process %u", HARNESS_FORK_PROCESS_TOTAL());                                 \
        }                                                                                                                          \
                                                                                                                                   \
        HARNESS_FORK_PROCESS_ID(HARNESS_FORK_PROCESS_TOTAL()) = forkSafe();                                                        \
                                                                                                                                   \
        if (HARNESS_FORK_PROCESS_ID(HARNESS_FORK_PROCESS_TOTAL()) == 0)                                                            \
        {                                                                                                                          \
            unsigned int HARNESS_FORK_PROCESS_IDX() = HARNESS_FORK_PROCESS_TOTAL();                                                \
                                                                                                                                   \
            if (pipeRequired)                                                                                                      \
            {                                                                                                                      \
                close(HARNESS_FORK_PARENT_READ_PROCESS(HARNESS_FORK_PROCESS_IDX()));                                               \
                close(HARNESS_FORK_PARENT_WRITE_PROCESS(HARNESS_FORK_PROCESS_IDX()));                                              \
            }

#define HARNESS_FORK_CHILD_END()                                                                                                   \
            if (HARNESS_FORK_PIPE_REQUIRED(HARNESS_FORK_PROCESS_IDX()))                                                            \
            {                                                                                                                      \
                close(HARNESS_FORK_CHILD_READ());                                                                                  \
                close(HARNESS_FORK_CHILD_WRITE());                                                                                 \
            }                                                                                                                      \
                                                                                                                                   \
            exit(0);                                                                                                               \
        }                                                                                                                          \
                                                                                                                                   \
        HARNESS_FORK_PROCESS_TOTAL()++;                                                                                            \
    }                                                                                                                              \
    while (0)

/***********************************************************************************************************************************
Process in the parent
***********************************************************************************************************************************/
#define HARNESS_FORK_PARENT_BEGIN()                                                                                                \
    do                                                                                                                             \
    {                                                                                                                              \
        for (unsigned int processIdx = 0; processIdx < HARNESS_FORK_PROCESS_TOTAL(); processIdx++)                                 \
        {                                                                                                                          \
            if (HARNESS_FORK_PIPE_REQUIRED(processIdx))                                                                            \
            {                                                                                                                      \
                close(HARNESS_FORK_CHILD_READ_PROCESS(processIdx));                                                                \
                close(HARNESS_FORK_CHILD_WRITE_PROCESS(processIdx));                                                               \
            }                                                                                                                      \
        }

#define HARNESS_FORK_PARENT_END()                                                                                                  \
        for (unsigned int processIdx = 0; processIdx < HARNESS_FORK_PROCESS_TOTAL(); processIdx++)                                 \
        {                                                                                                                          \
            if (HARNESS_FORK_PIPE_REQUIRED(processIdx))                                                                            \
            {                                                                                                                      \
                close(HARNESS_FORK_PARENT_READ_PROCESS(processIdx));                                                               \
                close(HARNESS_FORK_PARENT_WRITE_PROCESS(processIdx));                                                              \
            }                                                                                                                      \
        }                                                                                                                          \
    }                                                                                                                              \
    while (0)

/***********************************************************************************************************************************
End the fork block and check exit status for all child processes
***********************************************************************************************************************************/
#define HARNESS_FORK_END()                                                                                                         \
        for (unsigned int processIdx = 0; processIdx < HARNESS_FORK_PROCESS_TOTAL(); processIdx++)                                 \
        {                                                                                                                          \
            int processStatus;                                                                                                     \
                                                                                                                                   \
            if (waitpid(HARNESS_FORK_PROCESS_ID(processIdx), &processStatus, 0) != HARNESS_FORK_PROCESS_ID(processIdx))            \
                THROW_SYS_ERROR_FMT(AssertError, "unable to find child process %u", processIdx);                                   \
                                                                                                                                   \
            if (WEXITSTATUS(processStatus) != HARNESS_FORK_CHILD_EXPECTED_EXIT_STATUS(processIdx))                                 \
                THROW_FMT(AssertError, "child %u exited with error %d", processIdx, WEXITSTATUS(processStatus));                   \
        }                                                                                                                          \
    }                                                                                                                              \
    while (0)
