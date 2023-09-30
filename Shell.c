#include "Shell.h"
#include "CommandLine.h"
#include "String.h"
#include "Literals&Heap.h"

int main()
{
    CommandLine commandLine;
    char cmdLineStr[MAX_BUFFER_LEN];
    int validCmdLine, tokenizeError;

    initCmdLine(&commandLine);
    validCmdLine = readCmdLine(cmdLineStr);
    while (strncmp(cmdLineStr, exitMsg, strlen(exitMsg)) != 0 && cmdLineStr[strlen(exitMsg) != '\n'])
    {
        if (validCmdLine)
        {
            tokenizeError = tokenizeCommandLine(&commandLine, cmdLineStr);
            if (tokenizeError != -1)
            {
                tokenizeCommandAll(&commandLine);
                printCmdLine(&commandLine);
                // runCommands;
            }
            else
                write(1, invalidInputMsg, strlen(invalidInputMsg));
        }
        else
            write(1, bufferOverflowMsg, strlen(bufferOverflowMsg));

        initCmdLine(&commandLine);
        validCmdLine = readCmdLine(cmdLineStr);
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