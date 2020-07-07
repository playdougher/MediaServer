#include <iostream>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig){
    std::cout << "\nreceived signal: " << sig << std::endl;
}

int main(int argc, char *argv[]){
    struct sigaction act, oact;

    act.sa_handler = sig_handler;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);

    sigaction(SIGINT, &act, &oact);

    pause();
    return 0;
}


