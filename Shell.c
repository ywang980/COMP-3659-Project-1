#include "Shell.h"

int main()
{
    CommandLine commandLine;
    char commandLineStr[MAX_BUFFER_LEN];
    int validCommandLineLen;

    initializeCommandLine(&commandLine);
    validCommandLineLen = readCommandLine(commandLineStr);
    while (strncmp(commandLineStr, exitPrompt, strlen(exitPrompt)) != 0 && commandLineStr[strlen(exitPrompt) != '\n'])
    {
        if (validCommandLineLen)
        {
            if (tokenizeCommandLine(&commandLine, commandLineStr) != -1 && tokenizeCommandAll(&commandLine) != -1)
            {
                // printCommandLine(&commandLine);
                runCommandLine(&commandLine);
            }
            else
                printError(invalidInput);
        }
        else
            printError(bufferOverflow);

        initializeCommandLine(&commandLine);
        validCommandLineLen = readCommandLine(commandLineStr);
    }

    return 0;
}

int readCommandLine(char *commandLineStr)
{
    write(1, promptMsg, strlen(promptMsg));
    return readInput(commandLineStr);
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