#include "header.h"

void removeFile(char *filenm, int fileindex){
    //Three levels of nested loop is used to change the data in the corresponding mechanism
    for (int i=0; i<numOfWord; i++)
    {
        for(int j=0; j<wordList[i].infilenum; j++)
        {
            if(wordList[i].infile[j] == fileindex)
            {
                if (wordList[i].infile[j+1] != '\0')    //determine if it is the last data
                {
                    //Deletes the file address and moves subsequent data forward
                    for (int k=j; k<wordList[i].infilenum; k++)
                    {
                        wordList[i].infile[k] = wordList[i].infile[k+1];
                    }
                    wordList[i].infile[wordList[i].infilenum-1] = '\0';
                }
                else    //if it's the last data
                    wordList[i].infile[j]='\0';
                //Subtract one from the number of files in which the word appears
                wordList[i].infilenum--;
                if(!wordList[i].infilenum)  //if the word does not appear in any file
                {
                    numOfWord--;
                    if(wordList[i+1].infilenum != 0)
                    {
                        //Move the subsequent words forward
                        for(int k=i; k<numOfWord; k++)
                        {
                            wordList[k] = wordList[k+1];
                        }
                    }
                    memset((void *)&wordList[numOfWord-1], 0x0, sizeof(WORD));
                }
            }
        }
    }

    numOfFile--;
    if(filepathList[fileindex+1] != NULL)   //if it's not the last file
    {
        for(int i=fileindex; i<numOfFile; i++)
        {
            filepathList[i] = filepathList[i+1];
        }
        free(filepathList[numOfFile-1]);
    }
    else
        free(filepathList[fileindex]);
}

//The loop gets the file name from the Filelist and deletes what needs to be deleted
void fileRemove(char *filelist[], int num)
{
    struct stat s_buf;

    for (int j=0; j<num; j++)
    {
        //  Get the file information and put the information into s_buf
        if(stat(filelist[j], &s_buf))
        {
            fprintf(stderr, "The path dose not exist\n");
            exit(EXIT_FAILURE);
        }
        else if(S_ISDIR(s_buf.st_mode))
        {
            fprintf(stderr, "The path is a directory name\n");
            exit(EXIT_FAILURE);
        }
        else if(S_ISREG(s_buf.st_mode))
        {
            for (int i=0; i<numOfFile; i++)
            {
                //Look in the structure for the file name to delete
                if(strcmp(filepathList[i], filelist[j]) == 0)
                {
                    removeFile(filepathList[i] ,i); //i is the address of the file to be deleted
                }
            }  
        }
        else
        {
            fprintf(stderr, "Unknown file type\n");
            exit(EXIT_FAILURE);
        }
    }
}