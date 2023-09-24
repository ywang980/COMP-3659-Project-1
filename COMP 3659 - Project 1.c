#include <stdio.h>
#include <unistd.h>

#define MAX_ARGS 10
#define MAX_COMMANDS 10
#define MAX_BUFFER_LEN 2048
#define MAX_MEMORY 2048

char memory[MAX_MEMORY];
int currMemUse = 0;

const char *promptMsg = "\nEnter a command:\n\0 ";
const char *invalidInputMsg = "Invalid input, try again.\n\0";
const char *bufferOverflowMsg = "Input length exceeds buffer limit, try again.\n\0";
const char *exitMsg = "exit\0";

typedef struct Command
{
    char *cmdStr;
    char *argv[MAX_ARGS];
    int argc;
} Command;

typedef struct ReDirect
{
    int flag;
    char *filePath;
} ReDirect;

typedef struct CommandLine
{
    Command commands[MAX_COMMANDS];
    int commandc;
    ReDirect input;
    ReDirect output;
    int bgFlag;
} CommandLine;

void printCmdLine(CommandLine *);
void listArgV(CommandLine *);
void printArgV(Command *);

int readCmdLine(CommandLine *, char *);
void initCmdLine(CommandLine *);
int readInput(char *);
void flushInput(char *);

int strlen(const char *);
int strncmp(const char *str1, const char *str2, int num);
int strchr(const char *, int, char);
char *allocateStr(int);
void strncpy(const char *, char *, int);
int flushWhiteSp(const char *, int);

int tokenizeCommandLine(CommandLine *, const char *);
int isSpChar(char);
int findNextSpChar(const char *, int);
int validSpChar(const char *, char);
int findNextInput(const char *, int);

void processSpChar(CommandLine *, const char *, int, int, int, char *);
int spCharFlagsOff(CommandLine *);
void addCmd(CommandLine *, const char *, int, int);
void addFilePath(CommandLine *, const char *, char, int, int);
void toggleSpCharFlag(CommandLine *, char);
void updateValSpChar(char *, char);

void tokenizeCommandAll(CommandLine *);
void tokenizeCommand(Command *, const char *);
int findNextDelim(const char *, int);
void addCmdArg(Command *, const char *, int, int);

int main()
{
    CommandLine commandLine;
    char cmdLineStr[MAX_BUFFER_LEN];
    int validCmdLine, tokenizeError;

    validCmdLine = readCmdLine(&commandLine, cmdLineStr);
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

        validCmdLine = readCmdLine(&commandLine, cmdLineStr);
    }

    return 0;
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

int readCmdLine(CommandLine *commandLine, char *cmdLineStr)
{
    initCmdLine(commandLine);
    write(1, promptMsg, strlen(promptMsg));
    return readInput(cmdLineStr);
}

void initCmdLine(CommandLine *commandLine)
{
    commandLine->commandc = 0;
    commandLine->input.flag = 0;
    commandLine->input.filePath = NULL;
    commandLine->output.flag = 0;
    commandLine->output.filePath = NULL;
    commandLine->bgFlag = 0;
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
    read(0, buffer, 1);
    while (buffer[0] != '\n')
    {
        read(0, buffer, 1);
    }
}

int strlen(const char *str)
{
    int len = 0;
    while (str[len])
    {
        len++;
    }
    return len;
}

int strncmp(const char *str1, const char *str2, int num)
{
    for (int i = 0; i < num; i++)
    {
        if (!str1[i] && !str2[i])
            return 0;
        else if (!str1[i])
            return -1;
        else if (!str2[i])
            return 1;

        if (str1[i] > str2[i])
            return 1;
        else if (str2[i] > str1[i])
            return -1;
    }

    return 0;
}

int strchr(const char *str, int start, char target)
{
    for (int i = start; i < strlen(str); i++)
    {
        if (str[i] == target)
            return 1;
    }

    return 0;
}

char *allocateStr(int len)
{
    char *st = &memory[currMemUse];
    memory[currMemUse + len] = '\0';
    currMemUse += (len + 1);
    return st;
}

void strncpy(const char *src, char *dst, int len)
{
    int i = 0;
    for (; i < len; i++)
    {
        dst[i] = src[i];
    }

    dst[i] = '\0';
}

int flushWhiteSp(const char *str, int start)
{
    int curr = start;
    for (; str[curr] == ' '; curr++)
        ;
    return curr;
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

            processSpChar(commandLine, cmdLineStr, start, curr, next, valSpChar);
            start = next;
            curr = findNextSpChar(cmdLineStr, start);
            next = findNextInput(cmdLineStr, curr);

            if (next == -1)
                return -1;
        }
    }
    return 0;
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

void processSpChar(CommandLine *commandLine, const char *cmdLineStr, int start, int curr, int next, char *valSpChar)
{
    char spChar = cmdLineStr[curr];

    if (spCharFlagsOff(commandLine))
        addCmd(commandLine, cmdLineStr, start, curr - start);

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
}

int spCharFlagsOff(CommandLine *commandLine)
{
    return (!commandLine->input.flag && !commandLine->output.flag && !commandLine->bgFlag);
}

void addCmd(CommandLine *commandLine, const char *cmdLineStr, int start, int len)
{
    commandLine->commandc++;
    int cmdIndex = commandLine->commandc - 1;
    commandLine->commands[cmdIndex].cmdStr = allocateStr(len);
    strncpy(&cmdLineStr[start], commandLine->commands[cmdIndex].cmdStr, len);
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

void tokenizeCommandAll(CommandLine *commandLine)
{
    for (int i = 0; i < commandLine->commandc; i++)
    {
        commandLine->commands[i].argc = 0;
        tokenizeCommand(&(commandLine->commands[i]), commandLine->commands[i].cmdStr);
    }
}

void tokenizeCommand(Command *command, const char *cmdStr)
{
    int start = flushWhiteSp(cmdStr, 0);
    int nextDelim, len;

    while (cmdStr[start])
    {
        nextDelim = findNextDelim(cmdStr, start);
        len = nextDelim - start;
        addCmdArg(command, cmdStr, start, len);
        start = flushWhiteSp(cmdStr, nextDelim);
    }
}

int findNextDelim(const char *str, int start)
{
    int curr = start;
    for (; str[curr] && str[curr] != ' '; curr++)
        ;
    return curr;
}

void addCmdArg(Command *command, const char *cmdStr, int start, int len)
{
    command->argc++;
    int argIndex = command->argc - 1;
    command->argv[argIndex] = allocateStr(len);
    strncpy(&cmdStr[start], command->argv[argIndex], len);
}