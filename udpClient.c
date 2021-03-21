#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>

int main() {
    int udp_client = socket(PF_INET, SOCK_DGRAM, 0);
    char* msg = "hello world";
    struct sockaddr_in addr;
    int server_port = 4399;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);


    sendto(udp_client, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(addr));

    return 0;
}