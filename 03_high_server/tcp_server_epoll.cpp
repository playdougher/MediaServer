#include <fcntl.h>
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

    int flag = fcntl(sock_fd, F_GETFL, 0);
    fcntl(sock_fd, F_SETFL, flag);

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
    char recv_buffer[BUFFER_LEN] = {0};
    int accept_fd;

    const int MAX_EVENTS = 20;
    epoll_event ev, events[MAX_EVENTS];
    const int TIMEOUT = 500; 
    int event_number;

    int epoll_fd = epoll_create(256);
    ev.events = EPOLLIN;
    ev.data.fd = sock_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev);

    for(;;){
        event_number = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);

        for(int i=0; i<event_number; i++){
            if(events[i].data.fd == sock_fd){
                socklen_t sock_len = (socklen_t)sizeof(accept_addr);
                accept_fd = accept(sock_fd, (struct sockaddr *)&accept_addr, &sock_len);

                flag = fcntl(accept_fd, F_GETFL, 0);
                fcntl(accept_fd, F_SETFL, flag);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = accept_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accept_fd, &ev);

                std::cout << "connected.\n" << std::endl;
            }
            else if( events[i].events & EPOLLIN){

                accept_fd = events[i].data.fd;

                memset(recv_buffer, 0, BUFFER_LEN);
                ret = recv(accept_fd, recv_buffer, BUFFER_LEN, 0);
                if(ret == BUFFER_LEN){
                    std::cout << "some data are ignored..." << std::endl;
                }
                if (ret <= 0){
                    switch(errno){
                        //没数据了，等通知
                        case EAGAIN:
                            break;
                        case EINTR:
                            recv(accept_fd, recv_buffer, BUFFER_LEN, 0);
                            break;
                        default:
                            std::cout << "closing connection..." << std::endl;
                            close(accept_fd);

                            std::cout << "Client is closed. fd: " << accept_fd << std::endl;
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, accept_fd, &ev);
                    }
                }
                else{
                    std::cout << "\nreceived messages: " << recv_buffer ;
                    std::cout << "send back." << std::endl;
                    ret = send(accept_fd, recv_buffer, sizeof(recv_buffer), 0);
                }
            }
        }

    }
    std::cout << "quit server..." << std::endl;
    close(sock_fd);

    return 0;
}
