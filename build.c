#include "header.h"

/*  Check whether the file has been indexed,
    if not, add it to the filepathList.  */
int searchfile(char *filepathList[], char *path)
{
    int isFound = 0;
    int i;
    for(i=0; i<numOfFile; i++)
    {
        if(!strcmp(path, filepathList[i]))
        {
            isFound = -1;
            return isFound;
        }
    }
    if(!isFound)
    {
        filepathList[i] = strdup(path);
        numOfFile++;
        return i;
    }
    return 0;
}

/*  Check whether the word has been detected,
    if not, add it to the wordList,
    if yes, add the index of file which 
    to the word's array which used to store
    indexes of file that contains the word. */
void wordlist(char word[], int fileIndex)
{
    int isFound = 0;
    int i = 0;

    while(i < numOfWord)
    {
        if(!strcmp(word, wordList[i].isword))
        {
            isFound = 1;
            for(int j=0; ;j++)
            {
                if(wordList[i].infile[j] == fileIndex)
                    break;
                if(wordList[i].infile[j] == '\0')
                {
                    wordList[i].infile[j] = fileIndex;
                    wordList[i].infilenum++;
                    break;
                }
            }
        }
        i++;
    }
    if(!isFound)
    {
        strcpy(wordList[i].isword, word);
        wordList[i].infile[0] = fileIndex;
        wordList[i].infilenum++;
        numOfWord++;
        printf("%dword: %s\n", numOfWord-1, wordList[i].isword);
    }
}

//  When detected a regular file, open and read its word
int isFile(char *filepath, int length)
{
    printf("finding words in \"%s\"\n", filepath);

    FILE *fp;   //pointer to the regular file
    int c;  //store the character read from file
    int i = 0;  //index of the word[]
    int fileIndex = 0;  //index of file stores in the filepathList
    char word[25];  //store detected word temporarily
    char abspath[512];

    fp = fopen(filepath, "r");

    if(fp == NULL)
    {
        fprintf(stderr, "Error with opening the file\n");
        exit(EXIT_FAILURE);
    }

    memset(abspath, '\0', sizeof(abspath));
    realpath(filepath, abspath);
    
    fileIndex = searchfile(filepathList, abspath);  //add new filepath to the list
    if(fileIndex < 0)   //when the file has already been indexed, skip it
        return 0;

    while(!feof(fp))    //read from the file until it ends
    {
        c = fgetc(fp);  //read from the file character by character
        if(isalnum(c))  //if it's a alphanumeric character, add it to the word[]
        {
            word[i] = c;
            ++i;
        }
        /*  If it's not a alphanumeric character, means the word has came to the end
            check whether the length satisfy the requirments. If it is, add this word
            to wordList. At the same time, clean the temporary word[] and set the
            index back to 0. */
        else if(!isalnum(c) && strlen(word)>=length)
        {
            i = 0;
            wordlist(word, fileIndex);
            memset(word, '\0', sizeof(word));
        }
    }
    fclose(fp);

    if(strlen(word)>=length)    //in case the last word could not be detected
        wordlist(word, fileIndex);

    return 0;
}

//  When detected a directory, tranverse it and index every file
void isDir(char filelist[], int length)
{
    DIR *dp;
    struct dirent *filepath;
    struct stat s_buf;

    if ((dp = opendir(filelist)) == NULL)
    {
        fprintf(stderr, "Error with opening the directory\n");
        exit(EXIT_FAILURE);
    }

    /*  Readdir() must be called in a loop to read the entire directory of files,
        when finished, readdir() will return NULL. If not yet, let it continues. */
    while((filepath = readdir(dp)) != NULL)
    {
        char file_path[512];
        memset(file_path,'\0',512);
        strcpy(file_path,filelist);
        strcat(file_path,"/");
        strcat(file_path,filepath->d_name);

        //  Get the file information and put the information into s_buf
        if(stat(file_path,&s_buf) < 0)
        {
            printf("path does not exist\n");
            exit(EXIT_FAILURE);
        }
        printf("%s\n", file_path);
        //  Determine whether it's a regular file
        if(S_ISREG(s_buf.st_mode))
        {
            isFile(file_path, length);
            continue;
        }
        // Determine whether it's a directory
        if(S_ISDIR(s_buf.st_mode))
        {
            //  Ignore the current and parent directory
            if(0==strcmp("..", filepath->d_name) || 0==strcmp(".", filepath->d_name))
            continue;
            isDir(file_path, length);  //parse files recursively
            continue;
        }
    }
    closedir(dp);
}

//  Write to the trovefile with gzip
void writeTrove(char *filenm)
{
    printf("writing and compressing trove-file\n");

    int thepipe[2];

    if(pipe(thepipe) != 0)
    {
        fprintf(stderr, "Fail to creat pipe\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if(pid == -1)
    {
        fprintf(stderr, "Fail to fork()\n");
        exit(EXIT_FAILURE);
    }

    if(!pid)    //new child process
    {             
        // If the child process write to stdout, it will write to the pipe.         
        dup2(thepipe[0], 0);    
        // Close the read end of the pipe
        close(thepipe[1]);      

        /*  Create a file, write only, with permissions of 0666, and truncate the file length to 0.
            If the file not exists, it will be created automatically.
            If the file exists, it will be truncated.   */
        int fd = open(filenm, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            fprintf(stderr, "Fail to open the trovefile\n");
            exit(EXIT_FAILURE);
        }

        dup2(fd, 1);
        //  Run gzip, writing to trovefile, read from stdin (pipe)
        execl("/usr/bin/gzip", "gzip", "-f", NULL);
        fprintf(stderr, "Execl error\n");
        close(thepipe[0]);  //close the read end of pipe
    }

    if(pid)     //original parent process
    {   
        int stdout_fd = dup(1);
        // If write to stdout, it will write to the pipe
        dup2(thepipe[1], 1);   
        //  Parent will never read from pipe 
        close(thepipe[0]);      

        printf("%d\n", numOfFile);  //print the number of files

        for(int i=0; i<numOfFile; i++)  //print the path of files
            printf("%s\n", filepathList[i]);

        printf("%d\n", numOfWord);  //print the number of words

        /*  Print the word itself, the number of files 
            which contain the word and the file index. */
        for(int i=0; i<numOfWord; i++)
        {
            printf("%s %d ", wordList[i].isword, wordList[i].infilenum);
            for(int k=0; k<wordList[i].infilenum; k++)
                printf("%d ", wordList[i].infile[k]);
            printf("\n");
        }

        close(thepipe[1]);  //close the write end of pipe
        dup2(stdout_fd, 1); //recover stdout
        wait(0);    // wait for the child process to exit
    }
    summary(numOfFile, numOfWord);
}

/*  Build a new trovefile from the contents of a filelist.
    A new trovefile will replace an existing trovefile.  */
void build(char *filelist[], int length, char *filenm, int num)
{
    printf("building trove-file\n");

    struct stat s_buf;
    
    memset(wordList, '\0', 1000 * sizeof(WORD));

    for(int i=0; i<num; i++)
    {
        //  Get the file information and put the information into s_buf
        if(stat(filelist[i], &s_buf))
        {
            fprintf(stderr, "The path dose not exist\n");
            exit(EXIT_FAILURE);
        }
        else if(S_ISDIR(s_buf.st_mode))
        {
            isDir(filelist[i], length);
            continue;
        }
        else if(S_ISREG(s_buf.st_mode))
        {
            isFile(filelist[i], length);
            continue;
        }
        else
        {
            fprintf(stderr, "Unknown file type\n");
            exit(EXIT_FAILURE);
        }
    }
    writeTrove(filenm);
}
