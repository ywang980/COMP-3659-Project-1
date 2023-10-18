/**
 * Name(s): Andy Wang, Micheal Myer, Vincent Ha
 * Emails: ywang980@mtroyal.ca, mmyer488@mtroyal.ca, vha117@mtroyal.ca
 * Course: COMP 3659 - 001
 * Instructor: 	Marc Schroeder
 * Assignment: 	Assignment 01
 * Source File: CommandLine.c
 * Due Date: 	October 6, 2023
 *
 * Purpose:		A library with functions relating to the tokenization and operation
 * of a command line.
 *
 * Details:		A command line is defined as a complex structure with the following fields:
 *              -An array of commands, which is a struct itself
 *                  -Each command structure has 4 fields: the untokenized command string
 *                   (of 1 command only), an argument vector array, an argument counter,
 *                   and the process ID it will have once fork() has been called
 *              -A command counter
 *              -An input and output redirect structure, each of which has 2 fields:
 *                  -A flag indicating the presence of an input/output redirect
 *                  -The corresponding filepath to redirect to
 *              -A background flag
 *              -A 2D-array of pipes (column count is fixed at 2)
 *              -Counters for the pipes/child processes the parent process has opened/forked
 *
 * Assumptions and Limitations:
 *              -Valid input must be of the form: command_1 | command_n | < input.txt > output.txt &
 *                  -Pipelining, redirect, and background jobs must be separated by exactly
 *                  1 of the corresponding special characters ('|', '<', '>', '&')
 *                  -All commands must appear before the redirect/background section
 *                  -Redirect/background characters must appear in the following order:
 *                      input redirect --> output redirect --> background
 *                  -Command arguments must be separated by at least 1 whitespace
 *                  -The redirect file path may not contain whitespace
 *
 *              -In the event of invalid input, the shell will not explicitly state the source
 *               of the input error
 *              -Absolute file paths must be provided to actually run the commands themselves
 */

#include "CommandLine.h"

/*
    Function: initializeCommandLine

    Purpose: initialize default command line fields
*/
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

/*
    Function: tokenizeCommandLine

    Purpose: parse command line input into 4 possible components: individual commands,
    input redirect, output redirect, and background job

    Details: the general algorithm of this function is:
            -Find index of user input (start)
                -First character that isn't a special character or whitespace
            -Find index of special character immediately trailing this input (curr)
                -('|', '<', '>', '&') or a newline
            -Process the input from start to curr with relation to the special
            character at curr
            -Find index of next user input, repeat while this is possible (i.e. trailing special
            character isn't a newline)

            Throughout this process, the input is validated in these areas:
            -The next user input parsed is valid
            -Special characters respect sequence of: pipeline->input->output->background
                -Implemented by declaring an array where each index is a unique valid special character
                -When earlier special characters are no longer valid, the array is truncated
            -Command count within limit

    Input: commandLine: CommandLine struct to split input into
           commandLineStr: user input string

    Return: 0 on succesful tokenization
            -1 on error

    Assumptions and Limitations:
*/
int tokenizeCommandLine(CommandLine *commandLine, const char *commandLineStr)
{
    char valSpChar[] = {'|', '<', '>', '&', '\n', '\0'};
    int start = flushWhiteSp(commandLineStr, 0);

    if (isSpChar(commandLineStr[start]) && commandLineStr[start] != '\n') // Can't start with special char
        return -1;
    else
    {
        int curr = findNextSpChar(commandLineStr, start); // Find trailing special char
        int next = findNextInput(commandLineStr, curr);   // Find/validate next user input

        if (next == -1)
            return -1;
        while (start != next)
        {
            if (!validSpChar(valSpChar, commandLineStr[curr])) // Valid special character sequence
                return -1;
            if (processSpChar(commandLine, commandLineStr, start, curr, next, valSpChar) == -1) // commandc <= MAX_COMMANDS
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

/*
    Function: flushWhiteSp

    Purpose: from the specified starting point of a given string, find the next
    non-whitespace character

    Return: index of first non-whitespace character
*/
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

/*
    Function: findNextSpChar

    Purpose: from the specified starting point of a given string, find the next
    special character

    Return: index of first special character
*/
int findNextSpChar(const char *str, int start)
{
    int curr = start;

    for (; !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

/*
    Function: findNextInput

    Purpose: from the index of the special character, find and validate the next
    possible user input

    Details: validity of next user input depends on current special character
        -'&': whitespace/newline only
        -Any other special character: next non-whitespace character from curr isn't another
        special character (i.e. non-consecutive special characters)

    Return: index of next input's start on success
            index of special character if end of command line
            -1 if next input is invalid
*/
int findNextInput(const char *commandLineStr, int curr)
{
    if (commandLineStr[curr] == '\n') // end of command line
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

/*
    Function: processSpChar

    Purpose: process the input from start to curr with relation to the special character at curr

    Details:
        -Since commands must appear before redirect/background requests, can only attempt
        to add a command while all relevant flags in the CommandLine struct are off
        -Upon encountering a special character that isn't a pipeline, at least 1 flag
        will be turned on
            -List of valid special characters are truncated as appropriate
        -By extension, any non-whitespace, non-newline user input once a flag is on
        can only be a file path

    Input: commandLine: CommandLine struct to split input into
           commandLineStr: user input string
           start: start of user input
           curr: index of special character
           next: start of next user input - used to find the file path
           valSpChar: array of valid special characters

    Return: 0 on success
            -1 on error - only possible error is exceeding command limit when attempting to add a command
*/
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

/*
    Function: findfPathEnd

    Purpose: for an index denoting the start of a file path, find the index immediately
    following this file path

    Return: index of first whitespace/special character (open is whitespace sensitive)
*/
int findfPathEnd(const char *str, int start)
{
    int curr = start;
    for (; str[curr] != ' ' && !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

/*
    Function: addCommand

    Purpose: first allocate heap space, then add an untokenized command string to the CommandLine
    struct. The command string's length and starting index in the overall command line are supplied
    as parameters.

    Return: 0 on success
            -1 on command count overflow
*/
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

/*
    Function: addFilePath

    Purpose: identical to addCommand, but for the file path fields instead
*/
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

/*
    Function: validSpChar

    Purpose: ensure the current special character is valid (i.e. respects specified input sequence)
*/
int validSpChar(const char *valSpChar, char spChar)
{
    return mystrchr(valSpChar, 0, spChar);
}

/*
    Function: toggleSpCharFlag

    Purpose: set flag for specified special character in CommandLine struct
*/
void toggleSpCharFlag(CommandLine *commandLine, char spChar)
{
    if (spChar == '<')
        commandLine->input.flag = 1;
    else if (spChar == '>')
        commandLine->output.flag = 1;
    else if (spChar == '&')
        commandLine->bgFlag = 1;
}

/*
    Function: updateValSpChar

    Purpose: update list of valid special characters by truncating the valSpChar array.

    Details: since any special character that isn't a pipeline can only appear once,
    it is invalid once it has been processed. Furthermore, any special characters appearing
    earlier in the sequence are also invalid.
    
    For example, output redirect may be processed only once, after which both it and input redirect
    (which must appear before output redirect) are invalid.
*/
void updateValSpChar(char *valSpChar, char spChar)
{
    int i = 0;
    for (; valSpChar[i] != spChar; i++)
        valSpChar[i] = '0';
    valSpChar[i] = '0';
}

/*
    Function: tokenizeCommandAll

    Purpose: ?????

    Input: commandLine: command line containing command

    Return: -1 on error
            0 otherwise

    Assumptions and Limitations:
*/
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

/*
    Function: tokenizeCommand

    Purpose: ?????

    Input: command: command to be tokenized
           commandString:

    Return: -1 on error
            0 otherwise

    Assumptions and Limitations:
*/
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

/*
    Function: findNextDelim

    Purpose: find the distance from a point of the string to the next space

    Input: str: string to be checked
           start: start of string to be proccessed

    Return: return length of string

    Assumptions and Limitations:
*/
int findNextDelim(const char *str, int start)
{
    int curr = start;
    for (; str[curr] && str[curr] != ' '; curr++)
        ;
    return curr;
}

/*
    Function: addCommandArg

    Purpose: find the distance from a point of the string to the next space
    ???????????????
    Input: command:
           commandString:
           start: passed to next function
           len: passed to next function

    Return: -1 on error
            0 otherwise

    Assumptions and Limitations:
*/
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

/*
    Function: runCommandLine

    Purpose: Create a child proccess and run command in that proccess

    Input: commandLine: command line containing information on user input

    Return:

    Assumptions and Limitations:
*/
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

/*
    Function: createPipe

    Purpose: attempt to create pipe

    Input: commandLine: holds location of pipe
           pipeIndex: reference to which pipe is being interacted with

    Return: -1 on error
            0 otherwise

    Assumptions and Limitations:
*/
int createPipe(CommandLine *commandLine, int pipeIndex)
{
    int error = pipe(commandLine->pipes[pipeIndex]);
    if (error == -1)
        printError(pipeFail);
    else
        commandLine->pipesOpen++;

    return error;
}

/*
    Function: closePipe

    Purpose: close pipe

    Input: commandLine: holds location of pipe
           pipeIndex: reference to which pipe is being interacted with

    Return:

    Assumptions and Limitations:
*/
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

/*
    Function: reDirectRequired

    Purpose: check if command needs redirect

    Input: commandLine: reference to input and output flags
           commandIndex: reference to which command is being operated

    Return: ???????

    Assumptions and Limitations:
*/
int reDirectRequired(CommandLine *commandLine, int commandIndex)
{
    return ((commandIndex == 0 && commandLine->input.flag) ||
            (commandIndex == commandLine->commandc - 1 && commandLine->output.flag));
}

/*
    Function: reDirectCommand

    Purpose: send output of one proocess to the input of another

    Input: commandLine: reference to input and output location
           commandIndex: reference to which command is being operated

    Return: -1 on error
            0 otherwise

    Assumptions and Limitations:
*/
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

/*
    Function: pipeCommand

    Purpose: ???????????

    Input: commandLine: reference to pipe location
           commandIndex: reference to which command is being operated

    Return:

    Assumptions and Limitations:
*/
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

/*
    Function: closePipesParent

    Purpose: close pipes for parent proccess

    Input: commandLine: reference to which command is being operated

    Return:

    Assumptions and Limitations:
*/
void closePipesParent(CommandLine *commandLine)
{
    for (int i = 0; i < 2 && i < commandLine->pipesOpen; i++)
    {
        close(commandLine->pipes[i][INPUT]);
        close(commandLine->pipes[i][OUTPUT]);
    }
}

/*
    Function: waitForChildren

    Purpose: pause parent proccess

    Input: commandLine: keep track of number of childProcesses

    Return:

    Assumptions and Limitations:
*/
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
