#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int string_to_int(char *s) {
    int res = 0;
    while (*s) {
        res = 10 * res + ((*s) - '0');
        s++;
    }
    return res;
}

int main(int argc, char* argv[]) {
   int k = string_to_int(argv[1]);
   int PRINT = 0, NO_PRINT = 1;
   int state = PRINT;
   char *buffer = malloc(k + 1);
   int len = 0;

   while (1) {
       int read_result = read(0, buffer, k + 1 - len);
       if (read_result < 0) return 1;

       if (read_result > 0) {
           int i = len;
           for (; i < len + read_result; i++) {
               if (buffer[i] == '\n') {
                   buffer[i] = 0;
                   if (state == PRINT) {
                     write();
                     write();
                   } else if (state == NO_PRINT) {
                   }
               }
           }
       }
   }  
}
