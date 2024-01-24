#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define MAX_LINE 1024
#define MAX_LISTEN 128

/* 状态码 */
enum STATUS_CODE
{
    SUCCESS = 0,
    
};

/* 用单线程/进程实现并发*/
int main()
{
    /* 创建套接字 句柄 */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket error");
        exit(1);
    }

    /* 绑定本地ip 和端口 */
    struct sockaddr_in servaddr;
#if 0
    memset(&servaddr, 0, sizeof(servaddr));
#else
    bzero(&servaddr, sizeof(servaddr));
#endif
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0)
    {
        perror("bind error");
        exit(1);
    }

    /* 监听 */
    ret = listen(sockfd, MAX_LISTEN);
    if (ret < 0)
    {
        perror("listen error");
        exit(1);
    }

    fd_set readfds;
    /* 清空集合 */
    FD_ZERO(&readfds);
    /* 把监听的文件描述符添加到读集合中,让内核帮忙检测 */
    FD_SET(sockfd, &readfds);
#if 0
    /* 设置超时 */
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    // 下面第三个NULL设置为 &timeout, 返回值ret为0时即为超时
#endif
    
    int maxfd = sockfd; /* 最大文件描述符 */
    /* 主循环 */
    while(1)
    {
        /* 调用select函数 */
        ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0)
        {
            perror("select error");
            break;
        }

        /* 处理监听的文件描述符 */
        if (FD_ISSET(sockfd, &readfds))     // 判断是否是监听的文件描述符
        {
            /* 处理新连接 */
            struct sockaddr_in cliaddr;
            socklen_t clilen = sizeof(cliaddr);
            int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
            if (connfd < 0)
            {
                perror("accept error");
                break;
                /* 一般不会到这 */
            }
            /* 将新连接的文件描述符添加到读集合中 */
            FD_SET(connfd, &readfds);   // 把connfd添加到集合中
            /* 更新最大文件描述符 */
            maxfd = maxfd > connfd ? maxfd : connfd;
        }

        /* 处理已连接的文件描述符 程序到这里说明可能有通讯 */
        for (int idx = 3; idx <= maxfd; idx++)              // 遍历所有文件描述符,判断是否是客户端发来的数据
        {

            if (idx != sockfd && FD_ISSET(idx, &readfds))   // 判断是否是客户端发来的数据
            {
                /* 到这里说明一定有通讯(老客户) */
                /* 接收数据 */
                char buffer[MAX_LINE] = {0};
                int readBytes = recv(idx, buffer, MAX_LINE - 1, 0);
                if (readBytes < 0)
                {
                    perror("recv error");
                    FD_CLR(idx, &readfds);                      // 将句柄从监听集合中删除
                    close(idx);                                 // 关闭连接
                    continue;                                   // 继续循环下一个客户端连接
                }
                else if (readBytes == 0)
                {
                    printf("client closed\n");
                    FD_CLR(idx, &readfds);
                    close(idx);
                    continue;
                }
                else
                {
                    printf("recv buffer: %s\n", buffer);
                    
                    /* 处理数据 */
                    toupper(buffer);

                    /* 发送数据 */
                    send(idx, buffer, strlen(buffer), 0);
                    memset(buffer, 0, MAX_LINE);
                    continue;                                   // 继续循环下一个客户端连接
                }
            }
        }
    }

    /* 关闭文件描述符 */
    close(sockfd);

    return 0;
}