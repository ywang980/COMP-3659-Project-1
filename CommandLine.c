#include "CommandLine.h"

void initCmdLine(CommandLine *commandLine)
{
    commandLine->commandc = 0;
    commandLine->input.flag = 0;
    commandLine->input.filePath = NULL;
    commandLine->output.flag = 0;
    commandLine->output.filePath = NULL;
    commandLine->bgFlag = 0;
}

int tokenizeCommandLine(CommandLine *commandLine, const char *cmdLineStr)
{
    char valSpChar[] = {'|', '<', '>', '&', '\n', '\0'};

    int start = flushWhiteSp(cmdLineStr, 0);
    if (isSpChar(cmdLineStr[start]) && cmdLineStr[start] != '\n')
        return -1;
    else
    {
        int curr = findNextSpChar(cmdLineStr, start);
        int next = findNextInput(cmdLineStr, curr);

        // Non-consecutive special char
        if (next == -1)
            return -1;

        while (start != next)
        {
            // Valid special char
            if (!validSpChar(valSpChar, cmdLineStr[curr]))
                return -1;

            if (processSpChar(commandLine, cmdLineStr, start, curr, next, valSpChar) == -1)
                return -1;

            start = next;
            curr = findNextSpChar(cmdLineStr, start);
            next = findNextInput(cmdLineStr, curr);

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

int validSpChar(const char *valSpChar, char spChar)
{
    return strchr(valSpChar, 0, spChar);
}

int findNextInput(const char *cmdLineStr, int curr)
{
    if (cmdLineStr[curr] == '\n')
        return curr;

    int next = flushWhiteSp(cmdLineStr, curr + 1);
    int validNext;

    if (cmdLineStr[curr] == '&')
        validNext = (cmdLineStr[next] == '\n');
    else
        validNext = (!isSpChar(cmdLineStr[next]));

    if (validNext)
        return next;
    else
        return -1;
}

int processSpChar(CommandLine *commandLine, const char *cmdLineStr, int start, int curr, int next, char *valSpChar)
{
    char spChar = cmdLineStr[curr];
    int error = 0;

    if (spCharFlagsOff(commandLine))
        error = addCmd(commandLine, cmdLineStr, start, curr - start);

    if (spChar == '<' || spChar == '>')
    {
        int fPathEnd = findNextSpChar(cmdLineStr, next);
        addFilePath(commandLine, cmdLineStr, spChar, next, fPathEnd - next);
    }

    if (spChar != '|')
    {
        toggleSpCharFlag(commandLine, cmdLineStr[curr]);
        updateValSpChar(valSpChar, cmdLineStr[curr]);
    }

    return error;
}

int spCharFlagsOff(CommandLine *commandLine)
{
    return (!commandLine->input.flag && !commandLine->output.flag && !commandLine->bgFlag);
}

int addCmd(CommandLine *commandLine, const char *cmdLineStr, int start, int len)
{
    commandLine->commandc++;
    if (commandLine->commandc > MAX_COMMANDS)
    {
        write(1, commandCountOverflowMsg, strlen(commandCountOverflowMsg));
        return -1;
    }
    else
    {
        int cmdIndex = commandLine->commandc - 1;
        commandLine->commands[cmdIndex].cmdStr = allocateStr(len);
        strncpy(&cmdLineStr[start], commandLine->commands[cmdIndex].cmdStr, len);
    }
    return 0;
}

void addFilePath(CommandLine *commandLine, const char *cmdLineStr, char reDirSign, int start, int len)
{
    ReDirect *reDirect;
    if (reDirSign == '<')
        reDirect = &(commandLine->input);
    else if (reDirSign == '>')
        reDirect = &(commandLine->output);

    reDirect->filePath = allocateStr(len);
    strncpy(&cmdLineStr[start], reDirect->filePath, len);
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
        if (tokenizeCommand(&(commandLine->commands[i]), commandLine->commands[i].cmdStr) == -1)
            return -1;
    }

    return 0;
}

int tokenizeCommand(Command *command, const char *cmdStr)
{
    int start = flushWhiteSp(cmdStr, 0);
    int nextDelim, len;

    while (cmdStr[start])
    {
        nextDelim = findNextDelim(cmdStr, start);
        len = nextDelim - start;
        if (addCmdArg(command, cmdStr, start, len) == -1)
            return -1;
        start = flushWhiteSp(cmdStr, nextDelim);
    }

    return 0;
}

int findNextDelim(const char *str, int start)
{
    int curr = start;
    for (; str[curr] && str[curr] != ' '; curr++)
        ;
    return curr;
}

int addCmdArg(Command *command, const char *cmdStr, int start, int len)
{
    command->argc++;
    if (command->argc > MAX_ARGS)
    {
        write(1, commandArgOverflowMsg, strlen(commandArgOverflowMsg));
        return -1;
    }
    else
    {
        int argIndex = command->argc - 1;
        command->argv[argIndex] = allocateStr(len);
        strncpy(&cmdStr[start], command->argv[argIndex], len);
    }
    return 0;
}

// void runCommandAll(CommandLine *commandLine) {}

void runCommand(CommandLine *commandLine)
{
    int pid = fork();
    int status;

    if (pid == -1)
    {
        // print fork fail message
        write(1, forkFailMsg, strlen(forkFailMsg));
        return;
    }
    else if (pid == 0)
    {
        if (execve(commandLine->commands[0].argv[0], commandLine->commands[0].argv, NULL) == -1)
            write(1, execveFailMsg, strlen(execveFailMsg));
    }
    else
        waitpid(pid, &status, 0);
}

void printCmdLine(CommandLine *commandLine)
{
    printf("\ncommandc: %d\n", commandLine->commandc);
    listArgV(commandLine);
    printf("\ninput flag: %d, input filePath: %s\n", commandLine->input.flag, commandLine->input.filePath);
    printf("output flag: %d, output filePath: %s\n", commandLine->output.flag, commandLine->output.filePath);
    printf("background flag: %d\n", commandLine->bgFlag);
}

void listArgV(CommandLine *commandLine)
{
    for (int i = 0; i < commandLine->commandc; i++)
    {
        printf("\ncommand %d: %s, argc: %d\n", i, commandLine->commands[i].cmdStr, commandLine->commands[i].argc);
        printArgV(&(commandLine->commands[i]));
    }
}

void printArgV(Command *command)
{
    for (int i = 0; i < command->argc; i++)
        printf("argv %d: %s\n", i, command->argv[i]);
}