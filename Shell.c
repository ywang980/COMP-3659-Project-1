/**
 * Name(s): 	Andy Wang, Micheal Myer, Vincent Ha
 * Emails: 	ywang980@mtroyal.ca, mmyer488@mtroyal.ca, vha117@mtroyal.ca
 * Course: 	COMP 3659 - 001
 * Instructor: 	Marc Schroeder
 * Assignment: 	Assignment 01
 * Source File: 	Shell.c
 * Due Date: 	October 6, 2023
 *
 * Purpose:	A program that emulates the Linux shell
 *
 * Details:	Will prompt user for commands and upon recieving a valid common execute
 *               the corresponding instruction. This will continue until exit command is
 *               entered which will then cause the program to terminate.
 *
 *               Any invalid commands will print out a error message.
 *
 * Assumptions and Limitations:
 *              All valid user input will be specify the file path of the command
 */
#include "Shell.h"

int main()
{
    CommandLine commandLine;
    char commandLineStr[MAX_BUFFER_LEN];
    int validCommandLineLen;

    initializeCommandLine(&commandLine);
    validCommandLineLen = readCommandLine(commandLineStr);
    while (mystrncmp(commandLineStr, exitPrompt, mystrlen(exitPrompt)) != 0)
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

/*
    Function: readCommandLine

    Purpose: prompt user then call readInput to read user message

    Input:

    Return: pointer to where user input is stored

    Assumptions and Limitations:
*/
int readCommandLine(char *commandLineStr)
{
    write(1, promptMsg, mystrlen(promptMsg));
    return readInput(commandLineStr);
}

/*
    Function: readInput

    Purpose: read user message return succesful or failed read

    Input: buffer: buffer space to hold input

???	Return: 1 on succesful read
            0 otherwise

    Assumptions and Limitations:
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

    Purpose: empty out buffer

    Input: buffer: buffer to empty

    Return:

    Assumptions and Limitations:
*/
void flushInput(char *buffer)
{
    while (read(0, buffer, MAX_BUFFER_LEN) >= MAX_BUFFER_LEN)
        ;
}
