#ifndef LITERALS_H
#define LITERALS_H

#define MAX_BUFFER_LEN 2048
#define MAX_HEAP_LEN 2048

static char heap[MAX_HEAP_LEN];
static int currHeapUse = 0;

void resetHeap();

#endif