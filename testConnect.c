#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>

int main() {
    const char* ip = "127.0.0.1";
    int port = 8888;
    const char* msg = "hello world";

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    int client = socket(PF_INET, SOCK_STREAM, 0);

    int ret = connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret != 0) {
        printf("errno: %d\n", errno);
        return 0;
    }

    send(client, msg, 12, 0);
    return 0;
}