#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char ** argumentArray;
int argIndex;


void printAnswer(char * buf, int count) {
   int num = 0;
    while (num < count) {
        int cur = write(1, buf + num, count - num);
        if (cur > 0) {
            num += cur;
        }
    }
}

int findfirst(char separatorCharacter, char * buf, int sz) {
    int i;
    for(i = 0; i < sz; ++i)
        if(buf[i] == separatorCharacter)
            return i;
    return -1;
}

void runoptarg(char * buf, int len) {
    int pipefd[2];
    pipe(pipefd);
    
    if( !fork() ) {
        dup2(pipefd[1], 1);
        argumentArray[argIndex] = buf;
        execvp(argumentArray[0], argumentArray); 
        exit(1);
    }
    int status = 0;
    wait(&status);
    if(WIFEXITED(status) && (WEXITSTATUS( status ) == 0 )) {
        buf[len] = '\n';
        printAnswer(buf, len + 1);
    }
}

int main(int argc, char ** argv) 
{
    int res = 0;
    int crashFlag = 0;
    int b = 4096; 
    char separatorCharacter;
    while( ( res = getopt(argc, argv, "nzb:") ) != -1 )
    {
        switch (res) 
        {
        case 'n': crashFlag++;separatorCharacter='\n'; break;
        case 'z': crashFlag++;separatorCharacter =0; break;
        case 'b': b = atoi(optarg); break;
        default : return 1;
        }
    }
    if(crashFlag >= 2)
       abort();
    char * buf = (char *) malloc(b + 1);
    
    argumentArray = (char **) malloc(argc - optind + 1);
    int i;
    for(i = optind; i < argc; i++) {
        if(strcmp(argv[i], "{}") == 0)
            argIndex = i - optind;
        argumentArray[i - optind] = argv[i];
    }  
    argumentArray[argc - optind] = 0;
    
    int r = 0, len = 0, findPosition;
    
    int eof = 0;
    while( !eof ) {
        if(len == b)
           return 2; 
        r = read(0, buf + len, b - len);
        
        if(r == 0) {
            if(len == 0)
                return 3;
            if (len > 0 && len < b) {
                runoptarg(buf, len);
            }
            break;
        }
        len += r; 
        while( (findPosition = findfirst(separatorCharacter, buf, len)) >= 0) 	{
            buf[findPosition] = 0;
            runoptarg(buf, findPosition);
            memmove(buf, buf + findPosition + 1, len - findPosition - 1);
            len -= findPosition + 1;
        }  
    } 

    

    return 0;
}
