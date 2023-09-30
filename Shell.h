#ifndef SHELL_H
#define SHELL_h

#include <unistd.h>
#include "CommandLine.h"
#include "String.h"
#include "Literals&Heap.h"

int readCmdLine(char *);
int readInput(char *);
void flushInput(char *);

#endif