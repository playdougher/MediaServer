#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>
#include <fcntl.h>

int main (int argc, char *argv[]) {
    const int PORT = 8111;
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0) {
        std::cout << "create socket failed." << std::endl;
        exit(1);
    }


    int optval = 1;
    int ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT,
            &optval, (socklen_t)optval);
    if (ret < 0) {
        std::cout << "set option failed." << std::endl;
        exit(1);
    }

    // non-block
    int flag = fcntl(sock_fd, F_GETFD, 0);
    fcntl(sock_fd, F_SETFL, flag | O_NONBLOCK);

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = PORT;
    bzero(&(sock_addr.sin_zero), 8);

    ret = bind(sock_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    if (ret < 0) {
        std::cout << "bind failed." << std::endl;
        exit(1);
    }

    int backlog = 10;
    ret = listen(sock_fd, backlog);
    if (ret < 0) {
        std::cout << "listen error." << std::endl;
        exit(1);
    }

    struct sockaddr_in accept_addr;
    const size_t BUFFER_LEN = 1024;
    char data_buffer[BUFFER_LEN] = {0};
    int accept_fd;

    fd_set fd_sets;
    const int ACCEPT_SIZE = 1024;
    int accept_fds[ACCEPT_SIZE] = {};
    int max_fd = sock_fd;
    int curpos = -1;
    int event = 0;

    for(int i = 0; i < ACCEPT_SIZE; i++){
        accept_fds[i] = -1;
    }

    for(;;){
        FD_ZERO(&fd_sets);
        FD_SET(sock_fd, &fd_sets);
        
        for(int i = 0; i < ACCEPT_SIZE; i++){
            if(accept_fds[i] != -1){
                max_fd = std::max(max_fd, accept_fds[i]);
                FD_SET(accept_fds[i], &fd_sets);
            }
        }

        event = select(max_fd+1, &fd_sets, NULL, NULL, NULL);
        
        if(event < 0){
            std::cout << "Failed to use select()" << std::endl;
            break;
        }
        else if(event == 0){
            std::cout << "Timeout!" << std::endl;
            continue;
        }
        else{
            // 若有新连接来了
            if(FD_ISSET(sock_fd, &fd_sets)){
                int i = 0;
                for(; i < ACCEPT_SIZE; i++){
                    if(accept_fds[i] == -1){
                        curpos = i;
                        break;
                    }
                }

                if (i == ACCEPT_SIZE) {
                    std::cout << "Connection full.\n" << std::endl;
                    continue;
                }


                socklen_t sock_len = (socklen_t)sizeof(accept_addr);
                accept_fd = accept(sock_fd, (struct sockaddr *)&accept_addr, &sock_len);

                flag = fcntl(accept_fd, F_GETFD, 0);
                //fcntl(accept_fd, F_SETFD, flag | O_NONBLOCK);
                fcntl(accept_fd, F_SETFL, flag | O_NONBLOCK);
                accept_fds[curpos] = accept_fd;

                std::cout << "\nconnected.\n";
            }

            // 处理数据
            for(int i=0; i<ACCEPT_SIZE; i++){
                if(accept_fds[i]!=-1 and FD_ISSET(accept_fds[i], &fd_sets)){
                    memset(data_buffer, 0, BUFFER_LEN);
                    ret = recv(accept_fds[i], data_buffer, BUFFER_LEN, 0);
                    if(ret == 0) {
                        std::cout << "closing connection..." << std::endl;
                        close(accept_fds[i]);
                        accept_fds[i] = -1;
                    }
                    else{
                        std::cout << "\nreceived messages: " << data_buffer ;
                        std::cout << "send back." << std::endl;
                        ret = send(accept_fds[i], data_buffer, sizeof(data_buffer), 0);
                    }
                }
            }
        }
    }
    std::cout << "quit server..." << std::endl;
    close(sock_fd);
    return 0;
}
