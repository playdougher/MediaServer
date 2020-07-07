#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main (int argc, char *argv[]) {
    const int PORT = 8111;
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

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

    const size_t BUFFER_LEN = 1024;
    char recv_buffer[BUFFER_LEN] = {0};

    for(;;){
        std::cout << "connected.\n" << std::endl;

        for(;;){
            memset(recv_buffer, 0, BUFFER_LEN);

            socklen_t sock_len = (socklen_t)sizeof(sock_addr);
            ret = recvfrom(sock_fd, &recv_buffer, BUFFER_LEN, 0,
                    (struct sockaddr *)&sock_addr, &sock_len);

            if(ret <= 0) break;
            std::cout << "\nreceived messages: " << recv_buffer ;
            std::cout << "send back." << std::endl;

            ret = sendto(sock_fd, &recv_buffer, BUFFER_LEN, 0,
                    (struct sockaddr *)&sock_addr, sizeof(sock_addr));
        }
        std::cout << "closing connection..." << std::endl;
        close(sock_fd);
    }
    std::cout << "quit server..." << std::endl;
    return 0;
}
