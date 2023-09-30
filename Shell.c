#include "Shell.h"

int main()
{
    CommandLine commandLine;
    char cmdLineStr[MAX_BUFFER_LEN];
    int validCmdLineLen;
    // int validCmdLineLen, tokenizeError;

    initCmdLine(&commandLine);
    validCmdLineLen = readCmdLine(cmdLineStr);
    while (strncmp(cmdLineStr, exitMsg, strlen(exitMsg)) != 0 && cmdLineStr[strlen(exitMsg) != '\n'])
    {
        if (validCmdLineLen)
        {
            // tokenizeError = tokenizeCommandLine(&commandLine, cmdLineStr);
            if (tokenizeCommandLine(&commandLine, cmdLineStr) != -1 && tokenizeCommandAll(&commandLine) != -1)
            {
                printCmdLine(&commandLine);
                // runCommandAll
                runCommand(&commandLine);
            }
            else
                write(1, invalidInputMsg, strlen(invalidInputMsg));
        }
        else
            write(1, bufferOverflowMsg, strlen(bufferOverflowMsg));

        initCmdLine(&commandLine);
        validCmdLineLen = readCmdLine(cmdLineStr);
    }

    return 0;
}

int readCmdLine(char *cmdLineStr)
{
    write(1, promptMsg, strlen(promptMsg));
    return readInput(cmdLineStr);
}

int readInput(char *buffer)
{
    if (read(0, buffer, MAX_BUFFER_LEN) == MAX_BUFFER_LEN && buffer[MAX_BUFFER_LEN - 1] != '\n')
    {
        flushInput(buffer);
        return 0;
    }

    return 1;
}

void flushInput(char *buffer)
{
    while (read(0, buffer, MAX_BUFFER_LEN) >= MAX_BUFFER_LEN)
        ;
}