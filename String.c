/**
 * Name(s): Andy Wang, Micheal Myer, Vincent Ha
 * Emails: ywang980@mtroyal.ca, mmyer488@mtroyal.ca, vha117@mtroyal.ca
 * Course: COMP 3659 - 001
 * Instructor: 	Marc Schroeder
 * Assignment: 	Assignment 01
 * Source File: String.c
 * Due Date: 	October 6, 2023
 *
 * Purpose:	A library with various functions for string operation.
 *
 * Details:
 *
 * Assumptions and Limitations:
 *
 */
#include "String.h"

/*
    Function: mystrlen

    Purpose: count length of string
*/
int mystrlen(const char *str)
{
    int len = 0;
    while (str[len])
    {
        len++;
    }
    return len;
}

/*
    Function: mystrcmp

    Purpose: compare two strings lexicographically up to specified number of characters

    Return: 0 if strings are equal
            1 if str1 is bigger
            -1 if str2 is bigger
*/
int mystrncmp(const char *str1, const char *str2, int num)
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

/*
    Function: mystrchr

    Purpose: check if a string contains at least one instance of the specified character
    starting from the given index

    Return: 1 if character is in string
            0 otherwise
*/
int mystrchr(const char *str, int start, char target)
{
    for (int i = start; i < mystrlen(str); i++)
    {
        if (str[i] == target)
            return 1;
    }

    return 0;
}

/*
    Function: allocateStr

    Purpose: allocate heap space for string of specified length

    Return: pointer to string pointing at heap
*/
char *allocateStr(int len)
{
    char *st = &heap[currHeapUse];
    heap[currHeapUse + len] = '\0';
    currHeapUse += (len + 1);
    return st;
}

/*
    Function: mystrncpy

    Purpose: copy len bytes of src string to dst string
*/
void mystrncpy(const char *src, char *dst, int len)
{
    int i = 0;
    for (; i < len; i++)
    {
        dst[i] = src[i];
    }

    dst[i] = '\0';
}

/*
    Function: printError

    Purpose: print error message to user
*/
void printError(const char *msg)
{
    write(2, msg, mystrlen(msg));
}
