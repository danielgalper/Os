#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <deque>
#include <string>
#include <iostream>
#include <stdio.h>
#include<fcntl.h>
#include<sys/types.h>

int find_sep_character(char sep_character, char * buffer, int size)
{
    if (size > 0)
    {
        for( int i = 0; i < size - 1; i++)
        {
            if (buffer[i] == sep_character && buffer[i + 1] == sep_character)
            {
                return i + 1;
            }
        }
    }
    return -1;
}

void command_run(std::deque <char*> & v)
{
    char ** command = (char **) malloc((v.size() + 1) * (sizeof(char *)));
    for (int i = 0; i < v.size(); i++)
    {
        command[i] = &v[i][0];
    }
    command[v.size()] = NULL;
    int pid = fork();
    if (pid)
    {
        pid_t tpid;
        int status;
        do
        {
            tpid = wait(&status);
        }
        while (tpid != pid);
        free(command);
        _exit(0);
    }
    else
    {
        if (execvp(&v[0][0], command) == -1)
        {
            _exit(1);
        }
    }
}


void run(std::deque<std::deque<char*> > & v)
{

    if (v.size() < 1)
    {
        _exit(1);
    }
    if (v.size() == 1)
    {
        command_run(v[0]);
    }
    else
    {
        int pipefd[2];
        pipe(pipefd);

        if (fork())
        {
            dup2(pipefd[0], 0);
            close(pipefd[0]);
            close(pipefd[1]);
            v.pop_front();
            run(v);
        }
        else
        {
            dup2(pipefd[1], 1);
            close(pipefd[0]);
            close(pipefd[1]);
            command_run(v[0]);
        }
    }
}

int main(int argc, char ** argv)
{
    char * buffer;
    char c, sep_character = '\0';
    int buffer_size = 4096;
    int buffer_result;
    int count = 0;
    char ** command;
    int file = open(argv[1], O_RDONLY);
    buffer = (char *) malloc(buffer_size);
    while (1)
    {
        int eof = 0;
        int count = 0;
        while(!eof)
        {
            if (count >= buffer_size)
            {
                return -1;
            }
            buffer_result = read(file, buffer + count, buffer_size - count);
            if (buffer_result == 0)
            {
                eof = 1;
            }
            if (buffer_result < 0)
            {
                break;
            }
            int from = count;
            count += buffer_result;
            int pos;
            std::deque < std::deque < char* > > global_v;
            while ((pos = find_sep_character(sep_character, buffer, count - from)) >= 0)
            {
                char * temp_name = (char *) malloc(pos);
                if (buffer[0] == '\0')
                {
                    memcpy(temp_name, buffer + 1, pos - 1);
                }
                else
                {
                    memcpy(temp_name, buffer, pos);
                }
                std::deque<char*> v;
        int i;
        int tmp = 0;                
        for (i = 0; i < pos; i++) if (temp_name[i] == '\0'){
            char* temp = (char*)malloc(i - tmp + 1);
             memcpy(temp, temp_name + tmp, i - tmp);
             temp[i - tmp] = '\0'; v.push_back(temp);
             tmp = i + 1;
        }
                global_v.push_back(v);
                pos++;
                if (count - pos < 0)
                {
                    break;
                }
                memmove(buffer, buffer + pos, count - pos);
                from = 0;
                count -= pos;
            }
            run(global_v);
        }
        if (count > 0)
        {
            if (count + 1 >= buffer_size)
            {
                return -1;
            }
        }
        break;
    }
    free(buffer);
    return 0;
}
