#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>


/* 状态码 */
enum STATUS_CODE
{
    SUCCESS = 0,        // 成功
    FAILURE = -1,       // 失败
    ERROR = -2,         // 错误
    EXIT = -3,          // 退出
    TIMEOUT = -4,       // 超时
    NOT_FOUND = -5,     // 未找到
    DUPLICATE = -6,     // 重复
};


#define SERVER_PORT 8888        // 服务器端口号
#define MAX_LISENT_NUM 128      // 最大监听数
#define MAX_BUFFER_SIZE 1024    // 最大缓冲区大小

int sockfd;
int client_sockfd;
/* 回收资源 */
void sigHander()
{
    /* todo */
}

int main()
{
    /* 注册信号 */
    signal(SIGINT,sigHander);
    signal(SIGQUIT,sigHander);
    signal(SIGSTOP,sigHander);

    /* 创建socket套接字 */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        return FAILURE;
    }


    /* 绑定地址 */
    struct sockaddr_in addr;                
    // 也可以是struct sockaddr addr 但这个结构体比较难
    /* 清除脏数据 */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;                          // 指定类型为ipv4 一般都是这个 不用修改
    addr.sin_port = htons(SERVER_PORT);                 // 端口号 配置范围是1~65535; htons将主机字节序转换为网络字节序(转成大端序)
#if 1
    addr.sin_addr.s_addr = INADDR_ANY;                  // INADDR_ANY:指定地址为0.0.0.0的地址 即“所有地址”、“任意地址”
#elif 0
    addr.sin_addr.s_addr = inet_addr("172.16.157.11");   // 配置ip地址时要用inet_addr函数转换为网络字节序 "172.16.157.11"是我的私网ip地址
#else
    inet_pton(AF_INET, "172.16.157.11", &addr.sin_addr.s_addr); // 通过函数绑定
#endif
    int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("bind error");
        return FAILURE;
    }

    /* 监听 */
    ret = listen(sockfd, MAX_LISENT_NUM);
    if (ret == -1)
    {
        perror("listen error");
        return FAILURE;
    }

    /* 接收连接 */
    printf("accepting...\n");
    struct sockaddr_in client_addr;                     // 用来存放客户端的地址信息
    memset(&client_addr, 0, sizeof(client_addr));       // 清除脏数据
    socklen_t client_addr_len = sizeof(client_addr);    // 用来存放客户端地址信息的长度
    client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);      // 返回客户端套接字描述符，如果出错则返回-1，并设置errno为错误代码。
    if (client_sockfd == -1)
    {
        perror("accept error");
        return FAILURE;
    }

    char buffer[MAX_BUFFER_SIZE] = {0};     // 缓冲区
    char replyBuffer[MAX_BUFFER_SIZE] = {0};// 缓冲区
    int recv_len = 0;                       // 接收长度
    while(1)
    {
        printf("准备接收\n");
        recv_len = recv(client_sockfd, buffer, MAX_BUFFER_SIZE, 0);
        if (recv_len == -1)
        {
            perror("recv error");
            return FAILURE;
        }
        else if (recv_len == 0)
        {
            printf("client closed\n");
            break;
        }
        else
        {
            printf("recv data:%s\n", buffer);
            sleep(1);
        }
        sleep(3);
        /* 发送数据 */
        printf("准备发送\n");
        if (strcmp(buffer, "Hello World") == 0)
        {
            strncpy(replyBuffer, "Good Night World", MAX_BUFFER_SIZE);  
        }
        else if (strcmp(buffer, "Hello Server") == 0)
        {
            strncpy(replyBuffer, "Hello Client", MAX_BUFFER_SIZE);
            sleep(2);
            break;
        }
        ret = send(client_sockfd, replyBuffer, sizeof(replyBuffer), 0);
        if (ret == -1)
        {
            perror("send error");
            return FAILURE;
        }
        printf("send data: %s\n", replyBuffer);
        memset(buffer, 0, MAX_BUFFER_SIZE);
        memset(replyBuffer, 0, MAX_BUFFER_SIZE);
        sleep(1);
    }


    printf("准备关闭\n");
    /* 关闭连接 */
    close(client_sockfd);
    close(sockfd);
    printf("server exit\n");
#if 0
    /* 用联合体判断是小端还是大端 */
    union {
        short s;
        char c[sizeof(short)];
    } un;
    un.s = 0x0102;
    if (sizeof(short) == 2)
    {
        if (un.c[0] == 1 && un.c[1] == 2)
        {
            printf("big endian\n");
        }
        else if (un.c[0] == 2 && un.c[1] == 1)
        {
            printf("little endian\n");
        }
        else
        {
            printf("unknown\n");
            return FAILURE;
        }
    }
    else
    {
        printf("sizeof(short) = %d\n", sizeof(short));
        return FAILURE;
    }
#endif

    
    return 0;
}