/**
 * Name(s): 	Andy Wang, Michael Myer, Vincent Ha
 * Emails: 	    ywang980@mtroyal.ca, mmyer488@mtroyal.ca, vha117@mtroyal.ca
 * Course: 	    COMP 3659 - 001
 * Instructor: 	Marc Schroeder
 * Assignment: 	Assignment 01
 * Source File: Shell.c
 * Due Date: 	October 17, 2023
 *
 * Purpose:	    A program that emulates the Linux shell
 *
 * Details:	Will prompt user for commands and upon recieving a valid common execute
 *          the corresponding instruction. This will continue until exit command is
 *          entered which will then cause the program to terminate.
 *
 *          Invalid commands or improperly formatted command line input will print
 *          an appropriate error message.
 *
 * Assumptions and Limitations:
 *              Valid commands are assumed to include the full file path.
 *                  eg: instead of 'ls', '/usr/bin/ls'.
 */
#include "Shell.h"

int main()
{
    CommandLine commandLine;
    char commandLineStr[MAX_BUFFER_LEN];
    int validCommandLineLen;

    initializeCommandLine(&commandLine);
    validCommandLineLen = readCommandLine(commandLineStr);                   // check command line length does not exceed buffer limit
    while (mystrncmp(commandLineStr, exitPrompt, mystrlen(exitPrompt)) != 0) // must type exactly "exit" to terminate shell
    {
        if (validCommandLineLen)
        {

            if (tokenizeCommandLine(&commandLine, commandLineStr) != -1 && tokenizeCommandAll(&commandLine) != -1) // validate and tokenize user input
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

/*
    Function: readCommandLine

    Purpose: prompt for and attempt to read user input
*/
int readCommandLine(char *commandLineStr)
{
    write(1, promptMsg, mystrlen(promptMsg));
    return readInput(commandLineStr);
}

/*
    Function: readInput

    Purpose: read user input from standard input stream to supplied buffer

    Return: 1 on succesful read
            0 on buffer overflow
*/
int readInput(char *buffer)
{
    if (read(0, buffer, MAX_BUFFER_LEN) == MAX_BUFFER_LEN && buffer[MAX_BUFFER_LEN - 1] != '\n')
    {
        flushInput(buffer);
        return 0;
    }

    return 1;
}

/*
    Function flushInput

    Purpose: empty input buffer
*/
void flushInput(char *buffer)
{
    while (read(0, buffer, MAX_BUFFER_LEN) >= MAX_BUFFER_LEN)
        ;
}
