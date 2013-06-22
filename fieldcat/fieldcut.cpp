#include <unistd.h>
#include <string>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iostream>

void* malloc_(size_t size) {
    void *p = malloc(size);
    if (p == nullptr)
        exit(EXIT_FAILURE);
    return p;
}

void print(int fd, std::string const& str) {
    const char *buffer = str.c_str();
    size_t size = str.size();
    size_t begin = 0;
    while (begin < size) {
        int w = write(fd, buffer + begin, size - begin);
        if (w < 0)
            exit(EXIT_FAILURE);
        begin += w;
    }
}


const std::string BORDER = ";";

std::vector<std::string> delimiters;
std::vector<int> look_for;

const size_t BUFFER_SIZE = 4096;
char *buffer = (char*) malloc_(BUFFER_SIZE);

std::vector<std::string> to_output;


void output() {
    for (auto i = look_for.begin(); i != look_for.end(); i++) {
        print(STDOUT_FILENO, to_output[*i]);
    }
}

void cut() {
    to_output.resize(delimiters.size());
    size_t size = 0;
    size_t filled = 0;
    bool eof_reached = false;
    while (true) {
        if (!eof_reached) {
            int r = read(STDIN_FILENO, buffer + size, BUFFER_SIZE - size);
            if (r < 0)
                exit(EXIT_FAILURE);
            if (r == 0) {
                eof_reached = true;
            }
            size += r;
        };

        char *pos = strstr(buffer, delimiters[filled].c_str());
        if (pos == nullptr && !eof_reached)
            exit(EXIT_FAILURE);

        size_t length = pos - buffer;
        //if (length > size)
        //    continue;
        length += delimiters[filled].size();
        to_output[filled] = std::string(buffer, buffer + length);
        memmove(buffer, buffer + length, size - length);
        size -= length;

        if (++filled == delimiters.size()) {
            output();
            filled = 0;
        }
        if (eof_reached && size == 0) {
            break;
        }
    }
}

int main(int argc, char **argv) {
    int i = 1;
    while (i < argc) { // delim parsing
        if (argv[i] == BORDER) {
            i++;
            break;
        }
        delimiters.push_back(argv[i]);
        i++;
    }
    while (i < argc) {
        look_for.push_back( atoi(argv[i++]) - 1 );
    }
    
    cut();
    
    return 0;
}
