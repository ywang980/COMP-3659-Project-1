#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#define MAX_ARGS 20
#define MAX_COMMANDS 20

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "String.h"
#include "Literals&Heap.h"

// for debug only
#include <stdio.h>

typedef struct Command
{
    char *cmdStr;
    char *argv[MAX_ARGS + 1];
    int argc;
} Command;

typedef struct ReDirect
{
    int flag;
    char *filePath;
} ReDirect;

typedef struct CommandLine
{
    Command commands[MAX_COMMANDS + 1];
    int commandc;
    ReDirect input;
    ReDirect output;
    int bgFlag;
} CommandLine;

void initCmdLine(CommandLine *);

int tokenizeCommandLine(CommandLine *, const char *);
int flushWhiteSp(const char *, int);
int isSpChar(char);
int findNextSpChar(const char *, int);
int validSpChar(const char *, char);
int findNextInput(const char *, int);

int processSpChar(CommandLine *, const char *, int, int, int, char *);
int spCharFlagsOff(CommandLine *);
int addCmd(CommandLine *, const char *, int, int);
void addFilePath(CommandLine *, const char *, char, int, int);
void toggleSpCharFlag(CommandLine *, char);
void updateValSpChar(char *, char);

int tokenizeCommandAll(CommandLine *);
int tokenizeCommand(Command *, const char *);
int findNextDelim(const char *, int);
int addCmdArg(Command *, const char *, int, int);

void runCommand(CommandLine *);

// for debug only
void printCmdLine(CommandLine *);
void listArgV(CommandLine *);
void printArgV(Command *);

#endif