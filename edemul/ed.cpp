#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <stack>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/poll.h>
#include<list>
void safe_write(int fd, const char *buf, size_t len);
int safe_read(int fd, char *buf, size_t len);
char* safe_malloc(size_t size);
int safe_poll(pollfd fds[], int nfds, int timeout);
pid_t pid;
char *buf;
std::list<std::string> text;
int cur_pos;
std::string rem[6];

void handler(int)
{
    if (buf != nullptr)
    {
        free(buf);
    }
    kill(pid, SIGINT);
}
std::string exec( std::string cmd, std::string arg){
     if(cmd == "add") {
	auto p1 = text.begin();
	advance(p1,cur_pos);
	text.insert(p1,arg);
	return "element add";
	
     }
     
     
     if(cmd == "mv"){
	int i = atoi(arg.c_str());
	cur_pos = i;
	if(i > text.size())
	   return "incorrext index";
	return "nice move";
     }
     if(cmd == "del"){
	auto p1 = text.begin();
	advance(p1,cur_pos); 
	text.erase(p1);
	return "delete complete";
     }
     return "Uknown command";
     
}
void check(int len,int i, int fd){
    std::string s = rem[i];
    s += std::string(buf,buf + len);
    int index = 0;
    for(int i = 0; i < s.length(); i++){
	
	if(s[i] == '\n'){
	   std::string s1 = s.substr(index, i - index);
           std::size_t found = s1.find(" ");
      	   std::string cmd = s1.substr(0,found);
	   std::string arg = s1.substr(found, s1.length() - found);
	   std::string message = exec(cmd, arg);
	   const char* b = message.c_str();
	   safe_write(fd, b, message.length());
	    
	}
    }
}
int sci(int fd, char *buf, size_t len, int index)
{
    size_t read_count = 0;
    int curr_read = -1;
    bool flag = true;
    while (curr_read && flag && read_count < len)
    {
        curr_read = safe_read(fd, buf + read_count, len - read_count);
        read_count += curr_read;
	
	
	if(read_count == len - 1) {
	   rem[index] += std::string(buf,buf + len - 1);
	   break;
	}
	check(read_count, index, fd);
	
    }
    return 0;
}
int safe_poll(pollfd fds[], int nfds, int timeout)
{
    int count = poll(fds, nfds, timeout);
    if (count == -1)
    {
        perror("Error");
        std::exit(EXIT_FAILURE);
    }
    return count;
}

void safe_write(int fd, const char *buf, size_t len)
{
    size_t write_count = 0;
    while (write_count < len)
    {
        int curr_write = write(fd, buf + write_count, len - write_count);
        if (curr_write == -1)
        {
            std::exit(EXIT_FAILURE);
        }
        write_count += curr_write;
    }
}
/*
int check_command(std::string command)
{
    if (command == "add")
    {
        return 1;
    }
    if (command == "print" || command == "del")
    {
        return 2;
    }
    return 0;
}

int check_buf(std::string line)
{
    size_t pos_fst_wspace = line.find(" ", 0);
    if (pos_fst_wspace == std::string::npos)
    {
        return -1;
    }
    std::string command = line.substr(0, pos_fst_wspace);
    int ret = check_command(command);
    if (!ret)
    {
        return -1;
    }
    if (ret == 2)
    {
        std::string path = line.substr(pos_fst_wspace + 1,
                            line.length() - pos_fst_wspace - 2);
        if (!check_path(path))
        {
            return -1;
        }
    }
    if (ret == 1)
    {
        size_t pos_scn_wspace = line.find(" ", pos_fst_wspace + 1);
        if (pos_scn_wspace == std::string::npos)
        {
            return -1;
        }
        std::string path = line.substr(pos_fst_wspace + 1,
                        pos_scn_wspace - pos_fst_wspace - 1);
        if (!check_path(path))
        {
            return -1;
        }
        std::string s_tree = line.substr(pos_scn_wspace + 1,
                        line.length() - pos_scn_wspace - 2);
        ret = check_tree(s_tree);
        return ret;
    }
    return 0;
}*/
int safe_read(int fd, char *buf, size_t len)
{
    std::cout << "start to read" << std::endl;
    int read_count = read(fd, buf, len);
    std::cout << "end read" << std::endl;
    if (read_count < 0)
    {
        perror("Error");
        std::exit(EXIT_FAILURE);
    }
    return read_count;
}

char* safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == nullptr)
    {
        std::exit(EXIT_FAILURE);
    }
    return static_cast<char*>(ptr);
}
int main()
{
    pid = fork();
    if (pid)
    {
        signal(SIGINT, handler);
        std::cout << "Server starter with pid = " << pid << std::endl;
        int status;
        waitpid(pid, &status, 0);
        return 0;
    }
    setsid();
    addrinfo hints;
    
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    addrinfo *result;
    if (getaddrinfo(nullptr, "8442", &hints, &result) != 0)
    {
        std::exit(EXIT_FAILURE);
    }
    if (result == nullptr)
    {
        std::exit(EXIT_FAILURE);
    }

    int socket_fd;
    socket_fd = socket(result->ai_family, result->ai_socktype,
                        result->ai_protocol);
    if (socket_fd == -1)
    {
        std::exit(EXIT_FAILURE);
    }

    int sso_status = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
                &sso_status, sizeof(int)) == -1)
    {
        std::exit(EXIT_FAILURE);
    }

    if (bind(socket_fd, result->ai_addr, result->ai_addrlen) == -1)
    {
        std::exit(EXIT_FAILURE);
    }
    
    const int backlog = 5;
    if (listen(socket_fd, backlog) == -1)
    {
        std::exit(EXIT_FAILURE);
    }

    pollfd fd[backlog + 1];
    fd[0].fd = socket_fd;
    fd[0].events = POLLIN;
   
    const int timeout = -1;
    int clients = 1;
    
    const size_t buf_len = 1024 + 1;
    buf = safe_malloc(buf_len);
    while (true)
    {
        safe_poll(fd, clients, timeout);
        const char* message;
        for (int i = 1; i < clients; i++)
        {
            if (fd[i].revents & (POLLERR | POLLHUP))
            {
                fd[i].fd = -1;
                fd[i] = fd[clients - 1];
                clients -= 1;
                continue;
            }

            if (fd[i].revents & POLLIN)
            {
		std::cout << 1 << std::endl;
                int ret = sci(fd[i].fd, buf, buf_len,i);
                if (ret == -1)
                {
                    const std::string error_msg = "some problems\n";
                    safe_write(fd[i].fd, error_msg.c_str(), error_msg.size());
                    continue;
                }
                if (ret == 0)
                {
                    if (fd[i].events & POLLOUT)
                    {
                        fd[i].events = POLLOUT;
                    }
                    else
                    {
                        //fd[i].events = 0;
                    }
                }
               /* std::vector<std::string> cm = get_command(buf);
                for (size_t z = 0; z < cm.size(); z++)
                {
                    std::cout << "\'" << cm[z] << "\'" << std::endl;
                }
                if (cm[0] == com_add)
                {
                    message = head.add(cm[1], cm[2]);
                }
                else if (cm[0] == com_print)
                {
                    message = head.print(cm[1]);
                }
                else if (cm[0] == com_del)
                {
                    message = head.del(cm[1]);
                }
                safe_write(fd[i].fd, message, strlen(message));*/
		std::cout << buf;
		std::cout.flush();
            }
        }

        if (fd[0].revents && POLLIN)
        {
            int fd_acc = accept(socket_fd, result->ai_addr, &result->ai_addrlen);
            if (fd_acc == -1)
            {
                std::exit(EXIT_FAILURE);
            }
            fd[clients].fd = fd_acc;
            fd[clients].events = POLLIN | POLLERR | POLLHUP;
            clients += 1;
        }
    }
}
