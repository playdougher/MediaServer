#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
    int sock_fd;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::cout << "create socket failed." << std::endl;
        exit(-1);
    }

    int ret;
    struct sockaddr_in sock_addr;
    const int PORT = 8111;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_port = PORT;

    ret = connect(sock_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    if (ret < 0) {
        std::cout << "connect failed." << std::endl;
        exit(-1);
    }

    const size_t BUFFER_SIZE = 1024;
    char recv_buffer[BUFFER_SIZE] = "";
    char send_buffer[BUFFER_SIZE] = "";

    for(;;){
        memset(recv_buffer, 0, BUFFER_SIZE);

        std::cout << "\nintput messages you want to send : " << std::endl;
        fgets(send_buffer, sizeof(send_buffer), stdin);
        if(strncmp(send_buffer, "q", BUFFER_SIZE) ==
                strncmp(send_buffer, "quit", BUFFER_SIZE) == 0) {
            break;
        }

        ret = send(sock_fd, send_buffer, strlen(send_buffer), 0);
        if (ret < 0) {
            std::cout << "connection disconnect." << std::endl;
            exit(-1);
        }

        int data_len;
        data_len = recv(sock_fd, recv_buffer, BUFFER_SIZE, 0);
        recv_buffer[data_len] = '\0';
        std::cout << "message received: " << recv_buffer;
    }
    std::cout << "quit client... " << recv_buffer << std::endl;
    close(sock_fd);
    return 0;
}
            

