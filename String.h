#ifndef STRING_H
#define STRING_H

#include <unistd.h>
#include "Literals&Heap.h"

int strlen(const char *);
int strncmp(const char *str1, const char *str2, int num);
int strchr(const char *, int, char);
char *allocateStr(int);
void strncpy(const char *, char *, int);

#endif