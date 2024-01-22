#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define BUFFER_SIZE 1024
#define PORT 9999
#define SERVER_IP "172.16.157.11"

int main()
{
    /* 创建套接字 */
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket error");
        exit(1);
    }

    /* 初始化地址信息 */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    /* 发送数据 */
    char buffer[BUFFER_SIZE];
    printf("Enter a message to send to the server: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Message sent to the server: %s\n", buffer);

    /* 接收数据 */
    int server_addr_len = sizeof(server_addr);
    int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len);
    if (bytes_received < 0)
    {
        perror("recvfrom error");
        exit(1);
    }
    /* 结尾加结束符 */
    buffer[bytes_received] = '\0';
    printf("Message received from the server: %s\n", buffer);

    /* 关闭套接字 */
    close(sockfd);
    return 0;
    
}
