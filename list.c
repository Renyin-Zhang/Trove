#include "header.h"

//  Read from trovefile and store the information into structures
void readTrove(char *filenm)
{
    //  Check the trovefile exists
    struct stat s_buf;
    int thepipe[2];

    if(stat(filenm, &s_buf) < 0)
    {
        fprintf(stderr, "");
        exit(EXIT_FAILURE);
    }

    if(pipe(thepipe) != 0)
    {
        fprintf(stderr, "Fail to creat pipe\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if(pid == -1)
    {
        fprintf(stderr, "Fail to fork()\n"); // process creation failed
        exit(EXIT_FAILURE);
    }
    
    if(!pid)    //new child process
    {   
        // If the child process write to stdout, it will write to the pipe
        dup2(thepipe[1], 1);
        // Close the read end of the pipe
        close(thepipe[0]);
        
        //  Open a file, open for reading and writing.
        int fd = open(filenm, O_RDWR);
        if (fd == -1) {
            fprintf(stderr, "Fail to open the trovefile\n");
            exit(EXIT_FAILURE);
        }

        dup2(fd, 0);
        //  Run zcat, reading from trovefile, write to stdout (pipe)
        execl("/usr/bin/zcat", "zcat", NULL);
        fprintf(stderr, "Execl error\n");
        close(thepipe[1]);  //close the write end of pipe
    }

    if(pid)    //original parent process
    {       
        // If read from stdin, it will read from the pipe
        dup2(thepipe[0], 0);
        // Parent will never write to the pipe
        close(thepipe[1]);
        
        if(scanf("%d", &numOfFile) != 1)    //scanf the number of files
        {
            fprintf(stderr, "Fail to recognize the number of files\n");
            exit(EXIT_FAILURE);
        }
        
        for(int i=0; i<numOfFile; i++)  //scanf the filepath
        {
            char temp[512];
            if(scanf("%s\n", temp) != 1)
            {
                fprintf(stderr, "Fail to recognize the filepath\n");
                exit(EXIT_FAILURE);
            }
            filepathList[i] = strdup(temp);
        }

        if(scanf("%d", &numOfWord) != 1)    //scanf the number of words
        {
            fprintf(stderr, "Fail to recognize the number of words\n");
            exit(EXIT_FAILURE);
        }

        /*  Scanf the word itself, the number of files 
            which contain the word and the file index. */
        for(int i=0; i<numOfWord; i++)  
        {
            if(scanf("%s %d", wordList[i].isword, &wordList[i].infilenum) != 2)
            {
                fprintf(stderr, "Fail to recognize the word\n");
                exit(EXIT_FAILURE);
            }
            for(int j=0; j<wordList[i].infilenum; j++)
            {
                if(scanf("%d", &wordList[i].infile[j]) != 1)
                {
                    fprintf(stderr, "Fail to recognize the index\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        close(thepipe[0]);  // close the read end of the pipe
        
        wait(0);    // wait for the child process to exit.
    }
}

//  List the absolute pathname of files containing the indicated word
void list(char word[], int length, char *filenm)
{
    int SorF = 0;   //indicate whether finding the file contained the word sucessfully
    if(length > strlen(word))   //if the word's length shorter than standard, fail to search
    {
        fprintf(stderr, "The word's length does not meet the requirement\n");
        exit(EXIT_FAILURE);
    }

    readTrove(filenm);
    struct stat s_buf;
    for(int i=0; i<numOfWord; i++)  //compare the indicated word and words in wordList
    {
        if(strcmp(wordList[i].isword, word))
            continue;
        for(int j=0; j<wordList[i].infilenum; j++)
        {
            //  If the file does not exist, skip it
            if(stat(filepathList[wordList[i].infile[j]], &s_buf) < 0)
                continue;
            fprintf(stdout, "%s\n", filepathList[wordList[i].infile[j]]);
            SorF++;
        }
        break;
    }

    if(!SorF)   //if it can't find the word or the file does not exist
    {
        fprintf(stderr, "Fail to list files containing the indicated word\n");
        exit(EXIT_FAILURE);
    }
}
