#ifndef STRING_H
#define STRING_H

#include <unistd.h>
#include "Heap.h"

int strlen(const char *);
int strncmp(const char *, const char *, int);
int strchr(const char *, int, char);
char *allocateStr(int);
void strncpy(const char *, char *, int);
void printError(const char *);

#endif