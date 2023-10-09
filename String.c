#include "String.h"

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
    char *st = &heap[currHeapUse];
    heap[currHeapUse + len] = '\0';
    currHeapUse += (len + 1);
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

void printError(const char *msg)
{
    write(2, msg, strlen(msg));
}