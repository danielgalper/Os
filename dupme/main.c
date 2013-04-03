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
   printf("%d\n", k);
}
