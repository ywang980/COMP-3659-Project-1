/*******************************************************************************
* Name(s): 		Andy Wang, Michael Myer, Vincent Ha                            *
* Emails: 		ywang980@mtroyal.ca, mmyer488@mtroyal.ca, vha117@mtroyal.ca    *
* Course: 		COMP 3659 - 001                                                *
* Instructor: 	Marc Schroeder                                                 *
* Assignment: 	Assignment 01                                                  *
* Source File: 	CommandLine.c                                                  *
* Due Date: 	October 17, 2023                                               *
*                                                                              *
* Purpose:		A library with functions relating to the tokenization and      *
*               operation of a command line.                                   *
*                                                                              *
* Details:		A command line is defined as a complex structure with the      *
*               following fields:                                              *
*               -An array of commands, which is a struct itself                *
*                  -Each command structure has 4 fields: the untokenized       * 
*                   command string (of 1 command only), an argument vector     * 
*                   array, an argument counter, and the process ID it will     *
*                   have once fork() has been called                           *
*               -A command counter                                             *
*               -An input and output redirect structure, each of which has 2   *
*                fields:                                                       *
*                  -A flag indicating the presence of an input/output redirect *
*                  -The corresponding filepath to redirect to                  *
*               -A background flag                                             *
*               -A 2D-array of pipes (column count is fixed at 2)              *
*               -Counters for the pipes/child processes the parent process has * 
*                opened/forked                                                 *
*                                                                              *   
*                                                                              *
* Assumptions and Limitations:                                                 *
*               Valid commands follow the format:                              *
*                 command 1 (| ... | command n |) (< input) (> output) (&)     *
*               where                                                          *
*               - bracketed segments are optional and not mutually exclusive   *
*               - Pipelining, redirect, and background jobs must be separated  * 
*                 by exactly 1 of the corresponding special characters ('|',   *
*                 '<', '>', '&')                                               *
*               - All commands must appear before the redirect/background      *
*                 section                                                      *
*               - Redirect/background characters must appear in the following  *
*                 order: input redirect --> output redirect --> background     *
*               - Command arguments must be separated by at least 1 whitespace *
*               - The redirect file path may not contain whitespace            *
*               - Valid commands are assumed to include the full file path.    *
*                   eg: instead of entering 'ls', the user would enter         * 
*                       '/usr/bin/ls'.                                         *
*                                                                              *
*               In the event of invalid input, the shell will not explicitly   *
*               state the source of the input error                            *
*                                                                              *
*******************************************************************************/

#include "CommandLine.h"

/*       
*   Function: initializeCommandLine
*    
*	Purpose: initialize default command line fields
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
*   Function: tokenizeCommandLine
*    
*	Purpose:  parse command line input into 4 possible components: individual commands,
*             input redirect, output redirect, and background job 
*
*   Details: the general algorithm of this function is:
*            -Find index of user input (start)
*            -First character that isn't a special character or whitespace
*            -Find index of special character immediately trailing this input (curr)
*            -('|', '<', '>', '&') or a newline
*            -Process the input from start to curr with relation to the special
*             character at curr
*            -Find index of next user input, repeat while this is possible (i.e. trailing special
*             character isn't a newline)
*
*            Throughout this process, the input is validated in these areas:
*            -The next user input parsed is valid
*            -Special characters respect sequence of: pipeline->input->output->background
*            -Implemented by declaring an array where each index is a unique valid special character
*            -When earlier special characters are no longer valid, the array is truncated
*            -Command count within limit
*	
*	Inputs:   commandLine    - pointer to the CommandLine structure 
*             commandLineStr - user-entered string to be processed
*	
*	Return:   0 - on succesful tokenization
*            -1 - invalid command input format 
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
        int next = findNextInput(commandLineStr, curr); // Find/validate next user input

        if (next == -1) // consecutive special char
            return -1;
        while (start != next)
        {
            if (!validSpChar(valSpChar, commandLineStr[curr])) // Valid special character sequence
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
*   Function: flushWhiteSp
*    
*	Purpose:  remove whitespace from a string
*	
*	Return:   curr - index of next non-whitespace character
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
*   Function: findNextSpChar
*    
*	Purpose:  from the specified starting point of a given string, find the next
*             special character
*	
*	Return:   curr - index of the next special character
*/
int findNextSpChar(const char *str, int start)
{
    int curr = start;

    for (; !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

/*  
*   Function: findNextInput
*   
*	Purpose:  from the index of the special character, find and validate the next
*             possible user input
*	
*   Details: validity of next user input depends on current special character
*               - '&': whitespace/newline only
*               - Any other special character: next non-whitespace character from curr isn't another
*                 special character (i.e. non-consecutive special characters)
*	
*	Return:   index of next input's start on success
*             index of special character if end of command line
*             -1 if next input is invalid              
*/
int findNextInput(const char *commandLineStr, int curr)
{
    if (commandLineStr[curr] == '\n') // end of command line
        return curr; 

    int next = flushWhiteSp(commandLineStr, curr + 1);
    int validNext;

    if (commandLineStr[curr] == '&')
        validNext = (commandLineStr[next] == '\n'); // bg-job flag must be at the end of the command line
    else
        validNext = (!isSpChar(commandLineStr[next])); // consecutive special characters not allowed

    if (validNext)
        return next; // start of next command
    else
        return -1; // illegal character
}

/*
    Function: processSpChar

    Purpose:  process the input from start to curr with relation to the special character at curr

    Details:
              -Since commands must appear before redirect/background requests, can only attempt
               to add a command while all relevant flags in the CommandLine struct are off
              -Upon encountering a special character that isn't a pipeline, at least 1 flag
               will be turned on
              -List of valid special characters are truncated as appropriate
              -By extension, any non-whitespace, non-newline user input once a flag is on
               can only be a file path

    Input:    commandLine    - CommandLine struct to split input into
              commandLineStr - user input string
              start          - start of user input
              curr           - index of special character
              next           - start of next user input - used to find the file path
              valSpChar      - array of valid special characters

    Return:   0 - on success
             -1 - on error. The only possible error is exceeding command limit when attempting to add a command
*/
int processSpChar(CommandLine *commandLine, const char *commandLineStr, int start, int curr, int next, char *valSpChar)
{
    char spChar = commandLineStr[curr];
    int error = 0;

    if (spCharFlagsOff(commandLine)) //no redirects or background jobs
        error = addCommand(commandLine, commandLineStr, start, curr - start);

    if (spChar == '<' || spChar == '>') // handle io redirect
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
*   Function: spCharFlagsOFF
*    
*	Purpose: checks if input/output redirect and background job flags are set
*/
int spCharFlagsOff(CommandLine *commandLine)
{
    return (!commandLine->input.flag && !commandLine->output.flag && !commandLine->bgFlag);
}

/*       
*   Function: findfPathEnd
*    
*	Purpose:  for an index denoting the start of a file path, find the index immediately
*             following this file path
*	
*	Return:   curr - index of first whitespace/special character (open is whitespace sensitive)
*/
int findfPathEnd(const char *str, int start)
{
    int curr = start;
    for (; str[curr] != ' ' && !isSpChar(str[curr]); curr++)
        ;

    return curr;
}

/*     
*   Function: addCommand
*
*	Purpose:  first allocate heap space, then add an untokenized command string to the CommandLine
*             struct. The command string's length and starting index in the overall command line are supplied
*             as parameters.
*	
*	Input:    commandLine    - pointer to the CommandLine structure
*             commandLineStr - user-entered command line
*             start          - start index of command to be added
*             len            - number of characters in command
*	
*	Return:  -1 - signals that the CommandLine contains too many commands
*             0 - command successfully added to CommandLine
*/
int addCommand(CommandLine *commandLine, const char *commandLineStr, int start, int len)
{
    commandLine->commandc++;
    if (commandLine->commandc > MAX_COMMANDS) // max commands reached
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
*   Function: addFilePath
*
*	Purpose:  sets redirected filepath for io redirects
*	
*	Input:    commandLine    - pointer to CommandLine structure
*             commandLineStr - user-entered command line
*             start          - start index of filepath
*             len            - length of filepath name
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
*   Function: validSpChar
*    
*	Purpose:  ensure the current special character is valid (i.e. respects specified input sequence)
*	
*	Return:   0 - invalid character
*             1 - valid special character
*/
int validSpChar(const char *valSpChar, char spChar)
{
    return mystrchr(valSpChar, 0, spChar);
}

/*     
*   Function: toggleSpCharFlag
*
*	Purpose:  sets flag for io redirects or background job based on special character token
*	
*	Input:    commandLine - pointer to CommandLine structure          
*             spChar      - value of which special character flag to set
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
*   Function: updateValSpChar
*
*	Purpose: Clear characters from string until special character
*
*   Details: since any special character that isn't a pipeline can only appear once,
*            it is invalid once it has been processed. Furthermore, any special characters appearing
*            earlier in the sequence are also invalid.
*
*            For example, output redirect may be processed only once, after which both it and input redirect
*            (which must appear before output redirect) are invalid.
*	
*	Input:   valSpChar - String to be adjusted
*            spChar    - special character in string
*/
void updateValSpChar(char *valSpChar, char spChar)
{
    int i = 0;
    for (; valSpChar[i] != spChar; i++)
        valSpChar[i] = '0';
    valSpChar[i] = '0';
}

/*     
*   Function: tokenizeCommandAll
*
*	Purpose: iterates through command list to process each command
*	
*	Input: commandLine: pointer to CommandLine structure
*	
*	Return: -1 - error tokenizing one or more commands
*            0 - commands successfully tokenized
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
*    Function: tokenizeCommand
*
*	Purpose:   tokenizes arguments for a command
*
*	Input:     command       - command to be tokenized
*              commandString - string value of the command
*	
*	Return:   -1 - indicates an error tokenizing command's arguments
*              0 - command successfully tokenized
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
*   Function: findNextDelim
*
*	Purpose:  find the next whitespace character within a command
*	
*	Input:    str   - command string
*             start - index of command string to begin processing
*	
*	Return:   return index of the end of the current segment of a command
*/
int findNextDelim(const char *str, int start)
{
    int curr = start;
    for (; str[curr] && str[curr] != ' '; curr++)
        ;
    return curr;
}


/*     
*   Function: addCommandArg
*
*	Purpose:  add an argument token for a command
*
*	Input:    command       - pointer to Command object
*             commandString - string value of the command 
*             start         - index of argument within the command string to be processed
*             len           - length of command argument
*	
*	Return:  -1 - indicates maximum number of arguments reached for command
*             0 - argument successfully added
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
*   Function: runCommandLine
*
*	Purpose:  Create a child process to run each command in the command line
*
*	Input:    commandLine - pointer to CommandLine structure
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
*   Function: createPipe
*
*	Purpose:  attempt to create pipe
*
*	Input:    commandLine - pointer to CommandLine structure that holds the pipe
*             pipeIndex   - reference to which pipe is being interacted with
*	
*	Return:  -1 - error creating pipe
*             0 - pipe successfully created
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
*   Function: closePipe
*
*	Purpose:  closes the read and write ends of a pipe
*
*	Input:    commandLine - pointer to CommandLine structure that holds the pipe
*             pipeIndex   - reference to which pipe is being interacted with
*/
void closePipe(CommandLine *commandLine, int pipeIndex)
{
    close(commandLine->pipes[pipeIndex][INPUT]);
    close(commandLine->pipes[pipeIndex][OUTPUT]);
}


/*     
*   Function: runCommand
*
*	Purpose:  runs a single command, handling redirects as needed
*
*	Input:    commandLine  - pointer to CommandLine structure that holds the command
*             commandIndex - reference to which command is being interacted with
*
*   Return:  -1 - error running command
*             0 - command successfully run
*/
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
*   Function: reDirectRequired
*
*	Purpose:  check if command needs redirect
*
*	Input:    commandLine  - pointer to CommandLine structure containing redirect flags
*             commandIndex - reference to which command is being operated
*	
*	Return:   TRUE or FALSE indicating if redirect is required
*/
int reDirectRequired(CommandLine *commandLine, int commandIndex)
{
    return ((commandIndex == 0 && commandLine->input.flag) ||
            (commandIndex == commandLine->commandc - 1 && commandLine->output.flag));
}


/*     
*   Function: reDirectCommand
*
*	Purpose:  set input/output redirect for a command
*
*	Input:    commandLine  - pointer to CommandLine structure conatining input/output location
*             commandIndex - reference to which command is being redirected
*	
*	Return:  -1 - error redirecting io
*             0 - io successfully redirected
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
*   Function: pipeCommand
*
*	Purpose:  pipe the output of one command to the next input command in the pipeline
*
*	Input:    commandLine - pointer to CommandLine structure 
*             commandIndex - reference to which command is being piped
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
*   Function: closePipesParent
*
*	Purpose:  close all pipes in the parent process
*
*	Input:    commandLine - pointer to CommandLine structure that contains all the pipes
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
*   Function: waitForChildren
*
*	Purpose:  pause parent process until all child processes return
*
*	Input:    commandLine - pointer to CommandLine Structure 
*/
void waitForChildren(CommandLine *commandLine)
{
    int error = 0;
    int status;
    for (int i = 0; i < commandLine->childProcessCount; i++)
    {
        if (waitpid(commandLine->commands[i].pid, &status, 0) == -1)
            error = -1;
    }

    if (error == -1)
        printError(waitpidFail);
}

