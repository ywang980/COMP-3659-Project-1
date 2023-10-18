/**
* Name(s): 	Andy Wang, Micheal Myer, Vincent Ha
* Emails: 	ywang980@mtroyal.ca, mmyer488@mtroyal.ca, vha117@mtroyal.ca
* Course: 	COMP 3659 - 001
* Instructor: 	Marc Schroeder
* Assignment: 	Assignment 01
* Source File: 	String.c
* Due Date: 	October 6, 2023
*
* Purpose:	A library containing functions to interact with strings.
*??????
* Details:	
*
* Assumptions and Limitations: 
*              
*/
#include "String.h"

/*       
    Function: mystrlen
    
	Purpose: count length of string
	
	Input: str: string to be counter
	
	Return: amount of characters in string

    Assumptions and Limitations:
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
    
	Purpose: compare two strings lexicographically
	
	Input: str1: string to be compared
           str2: string to be compared
           num: number of characters to be compared
	
	Return: 0 if strings are equal
            1 if str1 os bigger
            -1 if str2 is bigger

    Assumptions and Limitations:
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
    
	Purpose: check if a string has a character
	
	Input: str: string to be checked
           start: point of string to start checking
           target: character that is being searched for
	
	Return: 1 if character is in string
            0 otherwise
            
    Assumptions and Limitations:
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
    
	Purpose: allocate space for string
	
	Input: len: length of string
	
	Return: pointer to string pointing at heap

    Assumptions and Limitations:
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
    
	Purpose: copy a string
	
	Input: src: string to be copied
           dst: destanation of copy
           len: length of string to be copied
	
	Return:

    Assumptions and Limitations:
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
	
	Input: msg: error message to be printed to screen
	
	Return:

    Assumptions and Limitations:
*/
void printError(const char *msg)
{
    write(2, msg, mystrlen(msg));
}
