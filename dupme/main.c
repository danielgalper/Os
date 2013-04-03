#include <unistd.h>
#include <stdlib.h>

int string_to_int(char *s) {
    int res = 0;
    while (*s) {
        res = 10 * res + ((*s) - '0');
        s++;
    }
    return res;
}

char *buffer;

void print_string(int count) {
    int was_writed = 0;
    while (was_writed < count) {
        was_writed += write(1, buffer + was_writed, count - was_writed);    
    }
}

int main(int argc, char* argv[]) {
   int k = string_to_int(argv[1]);
   int PRINT = 0, NO_PRINT = 1;
   int state = PRINT;
   int len = 0;
   
   buffer = malloc(k + 1);

   while (1) {
       int read_result = read(0, buffer + len, k + 1 - len);
       if (read_result < 0) return 1;

       if (read_result == 0) {
           break;
       }
       
       if (read_result > 0) {
           read_result += len;
           for (; len < read_result;) {
               if (buffer[len] == '\n') {
                   if (state == PRINT) {
                       print_string(len + 1);
                       print_string(len + 1);
                   } else if (state == NO_PRINT) {
                       state = PRINT;
                   }
                   memmove(buffer, buffer + len + 1, read_result - len - 1);
                   read_result = read_result - len - 1;
                   len = 0;
               } else {
                   len++;
               }
           }

           if (len == k + 1) {
               len = 0;
               state = NO_PRINT;
           }
       }
   }  
   free(buffer);
}
