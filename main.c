#include "header.h"

//  Report Error, exit with failure
void usage()
{
    printf("Usage: ./trove [-f filename] [-b | -r | -u] [-l length] filelist\n");
    printf("or     ./trove [-f filename] word\n");
    printf("\n");
    printf("where options are:\n");
    printf("-b		build a new trove-file\n");
    printf("-f filename	provide the name of the trove-file to be built or searched\n");
    printf("-l length	specify the minimum-length of words added to the trove-file\n");
    printf("-r		remove information from the trove-file\n");
    printf("-u		update information in the trove-file\n");
    exit(EXIT_FAILURE);
}

//  Clean the allocated space
void clean()
{
    for (int i = 0; i < numOfFile; i++) {
        free(filepathList[i]);
    }

    memset((void *)&wordList, 0x0, sizeof(WORD)*numOfWord);
}

// Print out the summary of trovefile
void summary(int filenum, int wordnum)
{
    if(filenum == 1 || filenum == 0)
        printf("%d unique file\n", filenum);
    else
        printf("%d unique files\n", filenum);
    if(numOfWord == 1 || numOfWord == 0)
        printf("%d unique word\n", wordnum);
    else
        printf("%d unique words\n", wordnum);
}

int main(int argc, char *argv[])
{
    int opt;
    int oper = 0;   //determine which function to be called
    int length = DEFAULT_LENGTH;    //minimum length of word to be recognized
    char *filenm = (char *)DEFAULT_NAME;    //name of trovefile
    char *input[NUM_OF_INPUT];  //store the input word or filelist

    opterr  =  0;
    //  DETERMINE WHICH, IF ANY, COMMAND-LINE SWITCHES WERE PROVIDED
    while( (opt = getopt(argc, argv, OPTLIST)) != -1) {
        switch (opt)
        {
            case 'b':
                oper = 1;
                break;
            case 'r':
                oper = 3;
                break;
            case 'u':
                oper = 5;
                break;
            case 'l':
                length = atoi(optarg);
                break;
            case 'f':
                filenm  =  strdup(optarg);
                break;
            default:
                fprintf(stderr, "Unrecognized character!\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
    if(argc <= 0 || (oper!=0 && oper%2==0)) {    //if oper!=0 && oper%2==0, means user input more than 1 option
        usage();
        exit(EXIT_FAILURE);
    }

    //  Store filelists or a word
    int num = 0;    //number of input filepaths
    while(optind < argc) {
        if(strlen(argv[optind]) >= length)
        {
            input[num] = argv[optind];
            num++;
        }
        ++optind;
    }

    switch(oper)
    {
        //  List the absolute pathname of files containing the indicated word
        case 0: 
            list(input[0], length, filenm);
            clean();
            break;
        //  Build a new trove-file from the contents of a filelist
        case 1: 
            build(input, length, filenm, num);
            clean();
            break;
        //  If any of the files appear in the trove-file, remove all of their information
        case 3: 
            fileRemove(input, num);
            writeTrove(filenm);
            clean();
            break;
        //  Update the trove-file with the contents of all files in the filelist
        case 5: 
            fileRemove(input, num);
            build(input, length, filenm, num);
            clean();
            break;
    }
    exit(EXIT_SUCCESS);
}
