#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#define MAX_ARGS 10
#define MAX_COMMANDS 10

typedef struct Command
{
    char *cmdStr;
    char *argv[MAX_ARGS];
    int argc;
} Command;

typedef struct ReDirect
{
    int flag;
    char *filePath;
} ReDirect;

typedef struct CommandLine
{
    Command commands[MAX_COMMANDS];
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

void processSpChar(CommandLine *, const char *, int, int, int, char *);
int spCharFlagsOff(CommandLine *);
void addCmd(CommandLine *, const char *, int, int);
void addFilePath(CommandLine *, const char *, char, int, int);
void toggleSpCharFlag(CommandLine *, char);
void updateValSpChar(char *, char);

void tokenizeCommandAll(CommandLine *);
void tokenizeCommand(Command *, const char *);
int findNextDelim(const char *, int);
void addCmdArg(Command *, const char *, int, int);

void printCmdLine(CommandLine *);
void listArgV(CommandLine *);
void printArgV(Command *);

#endif