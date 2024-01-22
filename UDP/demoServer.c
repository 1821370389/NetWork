#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
 #include <ctype.h>

#define BUFFER_SIZE 1024
#define PORT 9999

int main(int argc, char *argv[])
{
    /* 创建套接字 */
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket error");
        exit(1);
    }

    /* 初始化地址信息 */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* 绑定地址信息 */
    int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        perror("bind error");
        exit(1);
    }

    printf("Server is running...\n");

    /* 接收数据 */
    char buffer[BUFFER_SIZE];
    while (1)
    {
        /* 清空buffer */
        memset(buffer, 0, sizeof(buffer));

        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &len);
        if (n < 0)
        {
            perror("recvfrom error");
            exit(1);
        }
        printf("sockfd:%d\n", sockfd);
        printf("recv data: %s\n", buffer);
        for (int idx = 0; idx < n; idx++)
        {
            #if 0
            if(buffer[idx] >= 'a' && buffer[idx] <= 'z')
            {
                buffer[idx] -= 32;
            }
            #else
            toupper(buffer[idx]);

            #endif
        }


        sleep(2);
        /* 发送数据 */

        // char *data = "hello client";
        char *data = buffer;
        sendto(sockfd, data, strlen(data), 0, (struct sockaddr *)&client, len);
        printf("send data: %s\n", data);
        sleep(1);
        
        if(strncmp(buffer, "quit", n) == 0)
        {
            printf("Server quit\n");
            break;
        }
    }
    /* 关闭套接字 */
    close(sockfd);
    return 0;
}