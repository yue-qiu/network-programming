#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAXRECVLEN 1024

int main() {
    const char* ip = "127.0.0.1";
    int port = 8888;

    int sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    while (1) {
        pid_t pid;
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);

        int connfd = accept(sock, (struct sockaddr*)&client, &client_len);
        if (connfd == -1) {
            printf("errno is: %d\n", errno);
            exit(0);
        }
        if ((pid = fork()) == 0) {
            char remote[INET_ADDRSTRLEN];
            char msg[MAXRECVLEN];
            memset(msg, '\0', MAXRECVLEN);

            int data_len = recv(connfd, msg, MAXRECVLEN-1, 0);
            printf("remote address: %s:%d, msg:%s, data_len:%d\n", inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN), ntohs(client.sin_port), msg, data_len);
            close(connfd);
            exit(0);
        }
        close(connfd);        
    }
    
    shutdown(sock, SHUT_RDWR);
    return 0;
}