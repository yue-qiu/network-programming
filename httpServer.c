#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

const int BUFFER_SIZE = 2048;
const int MAX_EVENTS_NUM = 1024;

enum parse_line_status {OPEN = 0, OK, BAD};
enum parse_content_status {NO_REQUEST = 0, GET_REQUEST, BAD_REQUEST, INTERNAL_ERR};
enum check_status {PARSE_REQUESTLINE = 0, PARSE_HEADER};

/*
* 从 buffer 中解析一行
*/
int parseline(char* buffer, int buf_len, int* checked_idx) {
    while (*checked_idx < buf_len) {
        // 遇到 \r 字符
        if (buffer[*checked_idx] == '\r') {
            // 如果 \r 是 buffer 最后一个字符说明数据还没有完全读进 buffer 里
            if (*checked_idx + 1 == buf_len) {
                return OPEN;
            }
            // 如果 \r 后面跟着 \n，说明从 buffer 里读到了一行
            if (buffer[*checked_idx+1] == '\n') {
                buffer[*checked_idx] = '\0';
                ++(*checked_idx);
                buffer[*checked_idx] = '\0';
                ++(*checked_idx);
                return OK;
            }
            return BAD; // 否则说明请求格式有误
        }
        ++(*checked_idx);
    }
    return OPEN; // 把 buffer 读完也没发现 \r，说明还有更多数据等待被装到 buffer 里
}

/*
* 从 buffer 里解析请求行
*/
int parse_requestline(char* buffer, int* check_status) {
    char* req_path =  strpbrk(buffer, " \t");
    if (!req_path) {
        return BAD_REQUEST;
    }
    *req_path++ = '\0';

    char* req_method = buffer;
    
    req_path += strspn(req_path, " \t");

    char* req_version = strpbrk(req_path, " ");
    if (!req_version) {
        return BAD_REQUEST;
    }
    *req_version++ = '\0';

    req_version += strspn(req_version, " ");

    if (strcasecmp(req_version, "HTTP/1.1") != 0) {
        return BAD_REQUEST;
    }

    printf("%s %s %s\n", req_method, req_path, req_version);
    *check_status = PARSE_HEADER;
    return NO_REQUEST;
}


int parse_header(char* buffer) {
    if (buffer[0] == '\0') {
        return GET_REQUEST;
    }
    printf("%s\n", buffer);
    return NO_REQUEST;
}

int parse_content(char* buffer, int buf_len,  int* checked_idx, int* start_idx, int* check_status) {
    while (1) {
        int line_status = parseline(buffer, buf_len, checked_idx);
        if (line_status == OPEN) {
            return NO_REQUEST;
        } else if (line_status == BAD) {
            return BAD_REQUEST;
        }

        char* tmp_buffer = buffer + *start_idx;
        *start_idx = *checked_idx;

        int ret = NO_REQUEST;
        switch (*check_status)
        {
            case PARSE_REQUESTLINE:
                ret = parse_requestline(tmp_buffer, check_status);
                if (ret == BAD_REQUEST) {
                    return ret;
                }
                break;

            case PARSE_HEADER:
                ret = parse_header(tmp_buffer);
                if (ret == GET_REQUEST) return ret;
                break;
        
            default:
                return INTERNAL_ERR;
        }
    }
}

void addfd(int epoll_fd, int fd, int enbale_et) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enbale_et) {
        event.events |= EPOLLET;
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
}

void ET(int epoll_fd, struct epoll_event* events, int events_len, int listen_fd) {
    for (int i = 0; i < events_len; ++i) {
        if (events[i].data.fd == listen_fd) {
            struct sockaddr_in client;
            socklen_t client_len = sizeof(client);
            int connfd = accept(events[i].data.fd, (struct sockaddr*)&client, &client_len);
            assert(connfd != -1);
            addfd(epoll_fd, connfd, 1);
        } else if (events[i].events & EPOLLIN) {
            pid_t pid;
            if ((pid = fork()) == 0) {
                char buffer[BUFFER_SIZE]; // 读缓冲区
                memset(buffer, '\0', BUFFER_SIZE);
                int data_read = 0;
                int buf_len = 0;   
                int checked_idx = 0;
                int start_idx = 0;   
                int check_status = PARSE_REQUESTLINE;

                while (1) {
                    data_read = recv(events[i].data.fd, buffer + buf_len, BUFFER_SIZE - buf_len, 0); // 从连接 socket 中读数据
                    if (data_read == 0) {
                        printf("remote client has closed the connection\n");
                        close(events[i].data.fd);
                        return;
                    } else if (data_read < 0) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            printf("read later\n");
                            return;
                        }
                        printf("reading failed\n");
                        close(events[i].data.fd);
                        return;
                    }

                    buf_len += data_read;
                    int result = parse_content(buffer, buf_len, &checked_idx, &start_idx, &check_status);
                    if (result == NO_REQUEST) {
                        continue;
                    }

                    close(events[i].data.fd);
                    exit(0);
                }
            }
            close(events[i].data.fd);
        }
    }
}

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

    int epollfd = epoll_create(5);
    assert(epollfd != -1);

    struct epoll_event events[MAX_EVENTS_NUM];

    addfd(epollfd, sock, 0);

    while (1) {
        int ret = epoll_wait(epollfd, events, MAX_EVENTS_NUM, -1);
        assert(ret >= 0);

        ET(epollfd, events, ret, sock);
    }
    
    shutdown(sock, SHUT_RDWR);
    return 0;
}

