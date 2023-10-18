#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "String.h"

void testLength();
void testCompare();
void testChar();
void testCopy();

int main()
{
    int option = -1;

    while (option != 0)
    {
        printf("\nPlease select which function you'd like to test:\n%s\n%s\n%s\n%s", "1 - mystrlen", "2 - mystrncmp", "3 - mystrchr", "4 - mystrncpy");
        printf("\nOption #: ");
        scanf("%d", &option);

        switch(option)
        {
            case 1:
                testLength();
                break;

            case 2:
                testCompare();
                break;

            case 3:
                testChar();
                break;

            case 4:
                testCopy();
                break;

            default:
                printf("Please select an option from the four functions.");
        }
    }

    printf("Exiting test driver.");
    return 0;
}

void testLength()
{
    char string[25];
    int len;

    printf("\nNow testing mystrlen. Enter 'quit' to return to the main menu.\n");
    printf("\nEnter a string (max 25 characters): ");
    scanf("%s", string);

    while(strcmp(string, "quit") != 0)
    {
        len = mystrlen(string);

        printf("\nLength of string: %d characters\n", len);
        printf("\nEnter a string (max 25 characters): ");
        scanf("%s", string);
    }

    return;
}

void testCompare()
{
    char string1[25];
    char string2[25];
    int count;
    int result;

    printf("\nNow testing mystrncmp. Enter 'quit' to return to the main menu.\n");
    printf("\nEnter the first string (max 25 characters): ");
    scanf("%s", string1);
    printf("Enter the second string (max 25 characters): ");
    scanf("%s", string2);
    printf("Enter how many from the start of the string to compare: ");
    scanf("%d", &count);

    while(strcmp(string1, "quit") != 0)
    {
        result = mystrncmp(string1, string2, count);

        printf("\nResult: %d\n", result);
        printf("\nEnter the first string (max 25 characters): ");
        scanf("%s", string1);
        printf("Enter the first string (max 25 characters): ");
        scanf("%s", string2);
        printf("Enter how many from the start of the string to compare: ");
        scanf("%d", &count);
    }

    return;
}

void testChar()
{
    char string[25];
    char target;
    int start;
    int result;

    printf("\nNow testing mystrchr. Enter 'quit' to return to the main menu.\n");
    printf("\nEnter a string (max 25 characters): ");
    scanf("%s", string);
    printf("Enter a target character: ");
    scanf("%c", &target);
    scanf("%c", &target);
    printf("Enter starting index: ");
    scanf("%d", &start);

    while(strcmp(string, "quit") != 0)
    {
        result = mystrchr(string, start, target);

        printf("\nResult: %d\n", result);
        printf("\nEnter a string (max 25 characters): ");
        scanf("%s", string);
        printf("Enter a target character: ");
        scanf("%c", &target);
        scanf("%c", &target);
        printf("Enter starting index: ");
        scanf("%d", &start);

    }

    return;
}


void testCopy()
{
    char source[25];
    char dest[25];
    int length;

    printf("\nNow testing mystrncpy. Enter 'quit' to return to the main menu.\n");
    printf("\nEnter a string (max 25 characters): ");
    scanf("%s", source);
    printf("Enter number of characters to copy: ");
    scanf("%d", &length);

    while(strcmp(source, "quit") != 0)
    {
        mystrncpy(source, dest, length);

        printf("\nCopied string: %s\n", dest);
        printf("\nEnter a string (max 25 characters): ");
        scanf("%s", source);
        printf("Enter number of characters to copy: ");
        scanf("%d", &length);

    }

    return;
}

