#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>

const int DATALEN = 1024; 

int main() {
    int udp_server = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    int port = 4399;

    memset(&addr, 0, sizeof(addr));   
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    bind(udp_server, (struct sockaddr*)&addr, sizeof(addr));

    while (1) {
        char client_addr[INET_ADDRSTRLEN];
        struct sockaddr_in client;
        char msg[DATALEN];
        memset(msg, '\0', sizeof(msg));
        
        socklen_t client_len = sizeof(client);
        recvfrom(udp_server, msg, DATALEN, 0, (struct sockaddr*)&client, &client_len);
        printf("recieve data (%s) from %s:%d\n", msg, inet_ntop(AF_INET, &client.sin_addr, client_addr, INET_ADDRSTRLEN), ntohs(client.sin_port));
    }

    return 0;
}