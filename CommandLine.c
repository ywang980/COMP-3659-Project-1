/**
* Name(s): 		Andy Wang, Micheal Myer, Vincent Ha
* Emails: 		ywang980@mtroyal.ca, mmyer488@mtroyal.ca, vha117@mtroyal.ca
* Course: 		COMP 3659 - 001
* Instructor: 	Marc Schroeder
* Assignment: 	Assignment 01
* Source File: 	CommandLine.c
* Due Date: 	October 6, 2023
*
* Purpose:		A library with functions focused on proccessing the command line
*???????????
* Details:		Will split user input into tokens to be proccessed.
*
* Assumptions and Limitations: 
*              All valid user input will be specify the file path of the command
*/

#include "CommandLine.h"

/*       
    Function: initializeCommandLine
    
	Purpose: initialize values in commandLine
	
	Input: commnadLine: commandLine to be initialized
	
	Return: 

    Assumptions and Limitations:
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
    
	Purpose: split up command line input to proccesable pieces 
	???????
	Input: commnadLine: check max command size
           commandLineStr: string to be proccessed
	
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

/*       
    Function: flushWhiteSp
    
	Purpose: clear part of a string to the end
    
	Input: str: string that will be cleared
           start: starting point in string to be cleared
	
	Return: amount of string cleared

    Assumptions and Limitations:
*/
int flushWhiteSp(const char *str, int start)
{
    int curr = start;
    for (; str[curr] == ' '; curr++)
        ;
    return curr;
}

/*       
    Function: isSpChar
    
	Purpose: check which special character a character is
	
	Input: target: character to be checked
	
	Return: ASCII value of special character

    Assumptions and Limitations:
*/
int isSpChar(char target)
{
    return (target == '|' || target == '>' || target == '<' || target == '&' || target == '\n');
}

/*       
    Function: findNextSpChar
    
	Purpose: move through a string until a special character is hit
	
	Input: str: string to be searched through
           start: starting point of string
	
	Return: the amount of characters gone through

    Assumptions and Limitations:
*/
int findNextSpChar(const char *str, int start)
{
    int curr = start;

    for (; !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

/*       
    Function: findfPathEnd
    
	Purpose: go through string until a whitespace is hit
	
	Input: str: string to be proccessed through
           start: starting point of string
	
	Return: the amount of characters gone through

    Assumptions and Limitations:
*/
int findfPathEnd(const char *str, int start)
{
    int curr = start;
    for (; str[curr] != ' ' && !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

/*  ????????????
    Function: validSpChar
    
	Purpose: call mystrchr going through the beginning of string
	
	Input: valSpChar: string to be proccessed through
           spChar: spChar being searched for
	
	Return: position of spchar

    Assumptions and Limitations:
*/
int validSpChar(const char *valSpChar, char spChar)
{
    return mystrchr(valSpChar, 0, spChar);
}

/*  
    Function: findNextInput
    ???????????????????
	Purpose: 
	
	Input: 
	
	Return: 

    Assumptions and Limitations:
*/
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
/*  
    Function: processSpChar
    
	Purpose: sort proccessing depnding on special character recoeved
	
	Input: commandLine: Proccessing through command line
           commandLineStr: ?????????????
           start: passed to other functions
           curr: passed to other functions
           next: passed to other functions
           valSpChar: special character to be checked
	
	Return: 0 on success
            non-0 on error?????????

    Assumptions and Limitations:
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

/*       
    Function: spCharFlagsOFF
    
	Purpose: turn character flags off
	
	Input: commandLine: adjusting commandLine properties
	
	Return: ??????????????

    Assumptions and Limitations:
*/
int spCharFlagsOff(CommandLine *commandLine)
{
    return (!commandLine->input.flag && !commandLine->output.flag && !commandLine->bgFlag);
}

/*     
    Function: addCommand
    ??????????????????
	Purpose: 
	
	Input: commandLine: 
           commandLineStr:
           start:
           len:
	
	Return: -1 on error
            0 otherwise

    Assumptions and Limitations:
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

	Purpose: Adjust input or output of command
	
	Input: commandLine: to be changed
           commandLineStr: string containing command
           start: passed to next function
           len: passed to next function
	
	Return:
 
    Assumptions and Limitations:
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
    Function: toggleSpCharFlag

	Purpose: set flag for special character in command line
	
	Input: commandLine: to be changed
           commandLineStr: string containing command
           start: passed to next function
           len: passed to next function
	
	Return: 

    Assumptions and Limitations:
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

	Purpose: Clear characters from string until special character
	
	Input: valSpChar: String to be adjusted
           spChar: special character in string
	
	Return:

    Assumptions and Limitations:
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
