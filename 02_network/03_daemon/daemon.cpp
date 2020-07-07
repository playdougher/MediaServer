#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void daemon(){
    int id = fork();
    if (id < 0) {
        std::cout << "fork error!" << std::endl; 
        exit(-1);
    }
    
    // parent process
    if (id > 0) exit(0);

    //child process
    setsid(); //set session id
    if(chdir("/") < 0) {
        std::cout << "change directory error!" << std::endl; 
        exit(-1);
    }

    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    return ;
}

int main (int argc, char *argv[]) {
    daemon();

    while(1){
        sleep(1);
    }

    return 0;
}
