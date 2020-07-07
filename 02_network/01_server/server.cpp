/*
 * Server class
 *
 * @author wuzhihao
 * @date 2020-06-29
 * @copyleft GPL 2.0
 */

#include "server.h"
#include <unistd.h>
#include <iostream> 

namespace avdance{
    Server::Server(){
        std::cout << "construct..." << std::endl;
    }
    
    Server::~Server(){
        std::cout << "destruct..." << std::endl;
    }
    
    void Server::run(){
        while(1){
            std::cout << "running" << std::endl;
            ::usleep(1000000);
        }
    }
}

