#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#define MAX_ARGS 20
#define MAX_COMMANDS 20

#define INPUT 0
#define OUTPUT 1

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "String.h"
#include "Heap.h"

// for debug only
#include <stdio.h>

typedef struct Command
{
    char *commandString;
    char *argv[MAX_ARGS + 1];
    int argc;
    int pid;
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
    int pipes[MAX_COMMANDS][2];
} CommandLine;

void initializeCommandLine(CommandLine *);

int tokenizeCommandLine(CommandLine *, const char *);
int flushWhiteSp(const char *, int);
int isSpChar(char);
int findNextSpChar(const char *, int);
int findfPathEnd(const char *, int);
int validSpChar(const char *, char);
int findNextInput(const char *, int);

int processSpChar(CommandLine *, const char *, int, int, int, char *);
int spCharFlagsOff(CommandLine *);
int addCommand(CommandLine *, const char *, int, int);
void addFilePath(CommandLine *, const char *, char, int, int);
void toggleSpCharFlag(CommandLine *, char);
void updateValSpChar(char *, char);

int tokenizeCommandAll(CommandLine *);
int tokenizeCommand(Command *, const char *);
int findNextDelim(const char *, int);
int addCommandArg(Command *, const char *, int, int);

void runCommandLine(CommandLine *);
int createPipe(CommandLine *, int);
int createChildProcess(CommandLine *, int);

int runCommand(CommandLine *, int);
int reDirectRequired(CommandLine *, int);
int reDirectCommand(CommandLine *, int);
void pipeCommand(CommandLine *, int);

void closePipesParent(CommandLine *);
void cleanupChildren(CommandLine *);

// for debug only
void printCommandLine(CommandLine *);
void listArgV(CommandLine *);
void printArgV(Command *);

static const char *cmdCountOverflow = "\nCommand count exceeds maximum command limit.\n\0";
static const char *cmdArgOverflow = "\nArgument count exceeds maximum command limit.\n\0";

static const char *openFail = "\nopen failed. Command line discarded.\n\0";
static const char *pipeFail = "\npipe failed. Command line discarded.\n\0";
static const char *forkFail = "\nfork failed. Command line discarded.\n\0";
static const char *execveFail = "\nexecve failed. Command line discarded.\n\0";
static const char *waitpidFail = "\nwaitpid failed.\n\0";

#endif