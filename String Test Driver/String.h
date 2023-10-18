#ifndef STRING_H
#define STRING_H

#include <unistd.h>
#include "Heap.h"

int mystrlen(const char *);
int mystrncmp(const char *, const char *, int);
int mystrchr(const char *, int, char);
char *allocateStr(int);
void mystrncpy(const char *, char *, int);
void printError(const char *);

#endif