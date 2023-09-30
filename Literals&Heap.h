#ifndef LITERALS_H
#define LITERALS_H

#include <unistd.h>

#define MAX_BUFFER_LEN 2048
#define MAX_HEAP_LEN 2048

static const char *promptMsg = "\nEnter a command:\n\0 ";
static const char *invalidInputMsg = "Invalid input, try again.\n\0";
static const char *bufferOverflowMsg = "Input length exceeds buffer limit, try again.\n\0";
static const char *exitMsg = "exit\0";

static char heap[MAX_HEAP_LEN];
static int currHeapUse = 0;

void resetHeap();

#endif