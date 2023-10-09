#ifndef SHELL_H
#define SHELL_h

#include <unistd.h>
#include "CommandLine.h"
#include "String.h"
#include "Heap.h"

int readCommandLine(char *);
int readInput(char *);
void flushInput(char *);

static const char *promptMsg = "\nEnter a command:\n\0 ";
static const char *exitPrompt = "exit\0";

static const char *invalidInput = "\nInvalid input, try again.\n\0";
static const char *bufferOverflow = "\nInput length exceeds buffer limit, try again.\n\0";

#endif