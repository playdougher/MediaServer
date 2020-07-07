#include <iostream>

#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if (daemon(0,0) < 0) {
        std::cout << "create daemon error" << std::endl;
    }

    while(1){
        sleep(1);
    }
    return 0;
}


