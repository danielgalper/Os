#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
int b = 4096;
char split_char = '\n';
char *buffer;
char **args;
char **argv;
int argc;

void trythis(char *s) {
    int i = optind;
    args = malloc(sizeof(char*) * (argc + 1));
    for (; *(argv + i); i++) {
        if (strcmp(*(argv + i), "{}") == 0) {
            (*(args + i)) = s;
        } else {
            *(args + i) = *(argv + i);
        }
    }
    *(args + argc) = 0;
    int pid = fork();
    int devnull = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC
            | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    int pipefd[2];
    pipe(pipefd);
    if (pid == 0) {
      dup2(pipefd[1], 1); 
       execvp(args[optind], args + optind);
       exit(5);
    } 
    int status;
    waitpid(pid, &status, 0);
//    printf("blabla %s %d\n", s, status);
 //   printf("%d %d\n", WIFEXITED(status), WEXITSTATUS(status));
    if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) { 
       //  printf("%s\n", s);
        int slen = strlen(s);
        s[slen] = '\n';
        print_string(s, slen + 1);
    }
}

void print_string(char *buffer, int count) {
    int was_written = 0;
    while (was_written < count) {
        int cur = write(1, buffer + was_written, count - was_written);
        if (cur > 0) {
            was_written += cur;
        }
    }
}

int main(int argcc, char **argvv) {
    argc = argcc;
    argv = argvv;
    int c = 0;
    while ((c = getopt(argc, argv, "nzb:")) != -1) {
        switch (c) 
        {
            case 'n': split_char = '\n'; break;
            case 'z': split_char = 0; break;
            case 'b': b = atoi(optarg); break;
	    
            default:
                abort();
        }
    }
   
//    printf("%d\n", b);
    buffer = malloc(b + 1);
    int len = 0;
    while (1) {
        int read_result = read(0, buffer + len, b + 1 - len);
        
        if (read_result < 0) {
            return 1;
        }

        if (read_result == 0) {
            if (len != 0 && len <= b) {
                buffer[len] = 0;
                trythis(buffer);
            }
            break;
        }

        if (read_result > 0) {
            read_result += len;
            for (; len < read_result; ) {
                if (buffer[len] == split_char) {
                    buffer[len] = 0;
                    trythis(buffer);
                    memmove(buffer, buffer + len + 1, read_result - len - 1);
                    read_result = read_result - len - 1;
                    len = 0;
                } else {
                    len++;
                }
            }
            read_result += len;
        }
        
        if (len == b + 1) {
            return 1;
        }
    }        
}
