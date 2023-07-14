#ifndef   __HEADER_H__
#define   __HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#define _POSIX_C_SOURCE     200809L
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#define	OPTLIST             "f:brul:"
#define PATH_MAX_LENGTH     512
#define WORD_MAX_LENGTH     25
#define DEFAULT_NAME        "/tmp/trove"
#define DEFAULT_LENGTH      4
#define NUM_OF_WORDS        2000
#define NUM_OF_FILES        50
#define NUM_OF_INPUT        10


typedef struct Word
{
    char isword[WORD_MAX_LENGTH];    //store the word
    int infilenum;      //the num of files which have this word
    int infile[NUM_OF_FILES];     //the indexs of paths
} WORD;

static char *filepathList[NUM_OF_FILES];  //store all paths
static WORD wordList[NUM_OF_WORDS];     //store all words
static  int numOfWord;
static  int numOfFile;

void readTrove(char*);
void list(char[], int, char*);
int searchfile(char*[], char*);
void wordlist(char[], int);
int isFile(char*, int);
void isDir(char[], int);
void writeTrove(char*);
void build(char*[], int, char*, int);
void fileRemove(char *[], int);
void summary(int, int);

#endif
