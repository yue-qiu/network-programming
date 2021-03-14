#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

int main() {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address)); // struct sockaddr_in 置 0

    address.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr); // 把点分十进制地址转换为网络传输二进制地址
    address.sin_port = htons(atoi("8888")); // 把参数按照网络字节序进行转换

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address)); // 绑定 socket 到一个地址上
    assert(ret != -1);
    int backlog = 5;
    ret = listen(sock, backlog); // backlog 指定了监听队列最大长度。即最多有几个连接同时被建立（ESTABLISHED）。其余连接处于 SYN_RCVD 状态
    assert(ret != -1);
    while (1) {}
    
    return 0;
}