#include <signal.h>
#include <unistd.h>
#include <iostream>

void sig_handle(int sig){
    std::cout << "\nreceived signal: " ;
    if(sig == SIGINT)
        std::cout << "SIGINT" ;
    else if(sig == SIGQUIT)
        std::cout << "SIGQUIT" ;
    else if(sig == SIGHUP)
        std::cout << "SIGHUP" ;
    else 
        std::cout << sig ;
    std::cout << std::endl;
}

int main(int argc, char *argv[]){
    int sig;
    signal(SIGINT, sig_handle);
    signal(SIGQUIT, sig_handle);
    signal(SIGHUP, sig_handle);
    pause();
    return 0;
}


        
