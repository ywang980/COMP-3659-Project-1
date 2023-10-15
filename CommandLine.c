#include "CommandLine.h"

void initializeCommandLine(CommandLine *commandLine)
{
    commandLine->commandc = 0;
    commandLine->input.flag = 0;
    commandLine->input.filePath = NULL;
    commandLine->output.flag = 0;
    commandLine->output.filePath = NULL;
    commandLine->bgFlag = 0;
    commandLine->pipesOpen = 0;
    commandLine->childProcessCount = 0;
}

int tokenizeCommandLine(CommandLine *commandLine, const char *commandLineStr)
{
    char valSpChar[] = {'|', '<', '>', '&', '\n', '\0'};
    int start = flushWhiteSp(commandLineStr, 0);

    if (isSpChar(commandLineStr[start]) && commandLineStr[start] != '\n') // Can't start with special char
        return -1;
    else
    {
        int curr = findNextSpChar(commandLineStr, start);
        int next = findNextInput(commandLineStr, curr);

        if (next == -1) // Non-consecutive special char
            return -1;
        while (start != next)
        {
            if (!validSpChar(valSpChar, commandLineStr[curr])) // Valid special char
                return -1;
            if (processSpChar(commandLine, commandLineStr, start, curr, next, valSpChar) == -1) // Exceed max commands limit
                return -1;

            start = next;
            curr = findNextSpChar(commandLineStr, start);
            next = findNextInput(commandLineStr, curr);

            if (next == -1)
                return -1;
        }
    }
    return 0;
}

int flushWhiteSp(const char *str, int start)
{
    int curr = start;
    for (; str[curr] == ' '; curr++)
        ;
    return curr;
}

int isSpChar(char target)
{
    return (target == '|' || target == '>' || target == '<' || target == '&' || target == '\n');
}

int findNextSpChar(const char *str, int start)
{
    int curr = start;

    for (; !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

int findfPathEnd(const char *str, int start)
{
    int curr = start;
    for (; str[curr] != ' ' && !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

int validSpChar(const char *valSpChar, char spChar)
{
    return mystrchr(valSpChar, 0, spChar);
}

int findNextInput(const char *commandLineStr, int curr)
{
    if (commandLineStr[curr] == '\n')
        return curr;

    int next = flushWhiteSp(commandLineStr, curr + 1);
    int validNext;

    if (commandLineStr[curr] == '&')
        validNext = (commandLineStr[next] == '\n');
    else
        validNext = (!isSpChar(commandLineStr[next]));

    if (validNext)
        return next;
    else
        return -1;
}

int processSpChar(CommandLine *commandLine, const char *commandLineStr, int start, int curr, int next, char *valSpChar)
{
    char spChar = commandLineStr[curr];
    int error = 0;

    if (spCharFlagsOff(commandLine))
        error = addCommand(commandLine, commandLineStr, start, curr - start);

    if (spChar == '<' || spChar == '>')
    {
        int fPathEnd = findfPathEnd(commandLineStr, next);
        addFilePath(commandLine, commandLineStr, spChar, next, fPathEnd - next);
    }

    if (spChar != '|')
    {
        toggleSpCharFlag(commandLine, commandLineStr[curr]);
        updateValSpChar(valSpChar, commandLineStr[curr]);
    }

    return error;
}

int spCharFlagsOff(CommandLine *commandLine)
{
    return (!commandLine->input.flag && !commandLine->output.flag && !commandLine->bgFlag);
}

int addCommand(CommandLine *commandLine, const char *commandLineStr, int start, int len)
{
    commandLine->commandc++;
    if (commandLine->commandc > MAX_COMMANDS)
    {
        printError(cmdCountOverflow);
        return -1;
    }
    else
    {
        int commandIndex = commandLine->commandc - 1;
        commandLine->commands[commandIndex].commandString = allocateStr(len);
        mystrncpy(&commandLineStr[start], commandLine->commands[commandIndex].commandString, len);
    }
    return 0;
}

void addFilePath(CommandLine *commandLine, const char *commandLineStr, char reDirSign, int start, int len)
{
    ReDirect *reDirect;
    if (reDirSign == '<')
        reDirect = &(commandLine->input);
    else if (reDirSign == '>')
        reDirect = &(commandLine->output);

    reDirect->filePath = allocateStr(len);
    mystrncpy(&commandLineStr[start], reDirect->filePath, len);
}

void toggleSpCharFlag(CommandLine *commandLine, char spChar)
{
    if (spChar == '<')
        commandLine->input.flag = 1;
    else if (spChar == '>')
        commandLine->output.flag = 1;
    else if (spChar == '&')
        commandLine->bgFlag = 1;
}

void updateValSpChar(char *valSpChar, char spChar)
{
    int i = 0;
    for (; valSpChar[i] != spChar; i++)
        valSpChar[i] = '0';
    valSpChar[i] = '0';
}

int tokenizeCommandAll(CommandLine *commandLine)
{
    for (int i = 0; i < commandLine->commandc; i++)
    {
        commandLine->commands[i].argc = 0;
        if (tokenizeCommand(&(commandLine->commands[i]), commandLine->commands[i].commandString) == -1)
            return -1;
    }

    return 0;
}

int tokenizeCommand(Command *command, const char *commandString)
{
    int start = flushWhiteSp(commandString, 0);
    int nextDelim, len;

    while (commandString[start])
    {
        nextDelim = findNextDelim(commandString, start);
        len = nextDelim - start;
        if (addCommandArg(command, commandString, start, len) == -1)
            return -1;
        start = flushWhiteSp(commandString, nextDelim);
    }
    command->argv[command->argc] = NULL;

    return 0;
}

int findNextDelim(const char *str, int start)
{
    int curr = start;
    for (; str[curr] && str[curr] != ' '; curr++)
        ;
    return curr;
}

int addCommandArg(Command *command, const char *commandString, int start, int len)
{
    command->argc++;
    if (command->argc > MAX_ARGS)
    {
        printError(cmdArgOverflow);
        return -1;
    }
    else
    {
        int argIndex = command->argc - 1;
        command->argv[argIndex] = allocateStr(len);
        mystrncpy(&commandString[start], command->argv[argIndex], len);
    }
    return 0;
}

void runCommandLine(CommandLine *commandLine)
{
    int pid;

    for (int i = 0; i < commandLine->commandc; i++)
    {
        if (i > 1)
            closePipe(commandLine, i - 2);

        if ((i < commandLine->commandc - 1) && (createPipe(commandLine, i) == -1))
            goto cleanup;

        pid = fork();
        if (pid == -1)
        {
            printError(pipeFail);
            goto cleanup;
        }
        else if (pid == 0 && (runCommand(commandLine, i) == -1))
            _exit(0);
        else
        {
            commandLine->childProcessCount++;
            commandLine->commands[i].pid = pid;
        }
    }

cleanup:
    closePipesParent(commandLine);
    if (!commandLine->bgFlag)
        waitForChildren(commandLine);
}

int createPipe(CommandLine *commandLine, int pipeIndex)
{
    int error = pipe(commandLine->pipes[pipeIndex]);
    if (error == -1)
        printError(pipeFail);
    else
        commandLine->pipesOpen++;

    return error;
}

void closePipe(CommandLine *commandLine, int pipeIndex)
{
    close(commandLine->pipes[pipeIndex][INPUT]);
    close(commandLine->pipes[pipeIndex][OUTPUT]);
}

int runCommand(CommandLine *commandLine, int commandIndex)
{
    if (reDirectRequired(commandLine, commandIndex) && (reDirectCommand(commandLine, commandIndex) == -1))
        return -1;

    pipeCommand(commandLine, commandIndex);

    if (execve(commandLine->commands[commandIndex].argv[0], commandLine->commands[commandIndex].argv, NULL) == -1)
    {
        printError(execveFail);
        return -1;
    }

    return 0;
}

int reDirectRequired(CommandLine *commandLine, int commandIndex)
{
    return ((commandIndex == 0 && commandLine->input.flag) ||
            (commandIndex == commandLine->commandc - 1 && commandLine->output.flag));
}

int reDirectCommand(CommandLine *commandLine, int commandIndex)
{
    int fd;

    if (commandIndex == 0 && commandLine->input.flag)
    {
        fd = open(commandLine->input.filePath, O_RDONLY);
        if (fd == -1)
        {
            printError(openFail);
            return -1;
        }
        else
            dup2(fd, INPUT);
    }

    if (commandIndex == commandLine->commandc - 1 && commandLine->output.flag)
    {
        fd = open(commandLine->output.filePath, O_CREAT | O_RDWR, 0666);
        if (fd == -1)
        {
            printError(openFail);
            return -1;
        }
        else
            dup2(fd, OUTPUT);
    }

    return 0;
}

void pipeCommand(CommandLine *commandLine, int commandIndex)
{
    if (commandIndex < commandLine->commandc - 1)
    {
        close(commandLine->pipes[commandIndex][INPUT]);
        dup2(commandLine->pipes[commandIndex][OUTPUT], OUTPUT);
        close(commandLine->pipes[commandIndex][OUTPUT]);
    }

    if (commandIndex > 0)
    {
        close(commandLine->pipes[commandIndex - 1][OUTPUT]);
        dup2(commandLine->pipes[commandIndex - 1][INPUT], INPUT);
        close(commandLine->pipes[commandIndex - 1][INPUT]);
    }
}

void closePipesParent(CommandLine *commandLine)
{
    for (int i = 0; i < 2 && i < commandLine->pipesOpen; i++)
    {
        close(commandLine->pipes[i][INPUT]);
        close(commandLine->pipes[i][OUTPUT]);
    }
}

void waitForChildren(CommandLine *commandLine)
{
    int error = 0;
    int status;
    for (int i = 0; i < commandLine->childProcessCount; i++)
    {
        // printf("Waiting on child process: %d with pid: %d\n", i, commandLine->commands[i].pid);
        if (waitpid(commandLine->commands[i].pid, &status, 0) == -1)
            error = -1;
        // printf("Cleaned up child process: %d with pid: %d\n", i, commandLine->commands[i].pid);
    }

    if (error == -1)
        printError(waitpidFail);
}

/*
void printCommandLine(CommandLine *commandLine)
{
    printf("\ncommandc: %d\n", commandLine->commandc);
    listArgV(commandLine);
    printf("\ninput flag: %d, input filePath: %s\n", commandLine->input.flag, commandLine->input.filePath);
    printf("output flag: %d, output filePath: %s\n", commandLine->output.flag, commandLine->output.filePath);
    printf("background flag: %d\n\n", commandLine->bgFlag);
}

void listArgV(CommandLine *commandLine)
{
    for (int i = 0; i < commandLine->commandc; i++)
    {
        printf("\ncommand %d: %s, argc: %d\n", i, commandLine->commands[i].commandString, commandLine->commands[i].argc);
        printArgV(&(commandLine->commands[i]));
    }
}

void printArgV(Command *command)
{
    for (int i = 0; i < command->argc; i++)
        printf("argv %d: %s\n", i, command->argv[i]);
}
*/