#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#define SERVER_PORT 8080      //监听端口
#define BACKLOG     10        //最大连接数
#define BYE         "bye"     //断开连接暗号

/**
 * 多线程版本tcp连接
 */

int ret;      //没什么用，方便判断返回值

int main() {

    printf("starting tcp server\n");
    int server_socket;                  //监听请求socket
    int new_socket;                     //数据传输socket，由server_socket创建
    struct sockaddr_in server_addr;     //指定服务器绑定地址
    struct sockaddr_in client_addr;     //保存客户端地址
    int client_num = 0;                 //连接计数

    //创建socket，IPV4连接，启动TCP连接
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("create socket error\n");
        return -1;
    }
    printf("success to create socket\n");

    //设置socket_addr
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    //绑定端口
    ret = bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (ret == -1) {
        printf("bind error\n");
        return -1;
    }

    //设置监听信息
    ret = listen(server_socket, BACKLOG);
    if (ret == -1) {
        printf("listen error\n");
        return -1;
    }

    //阻塞监听请求，并创建业务线程
    while (true) {
        int addr_len = sizeof(struct sockaddr);
        //等待建立三次握手
        printf("server is listening on port %d\n", SERVER_PORT);
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (new_socket != -1) {
            printf("connect success");
            client_num ++;
            printf("Get connect from client %d : %s\n",  client_num, inet_ntoa(client_addr.sin_addr));
            //fork会创建子线程，两次返回，写时复制
            if (!fork()) {
                /* 子线程代码 */
                while (true) {
                    int recv_len;                       //返回长度
                    unsigned char recv_buf[1024];       //接收数据区

                    //接收数据，会阻塞
                    recv_len = recv(new_socket, recv_buf, sizeof(recv_buf)-1, 0);
                    if (recv_len < 0) {
                        printf("read client socket file error\n");
                        close(new_socket);
                        return -1;
                    }
                    recv_buf[recv_len] = '\0';

                    //打印接收的字符串
                    printf("Get Msg From Client %d: %s\n", client_num, recv_buf);

                    //判断是否断开连接
                    if (strcmp(recv_buf, BYE)) {
                        break;
                    }

                    //发送一些数据给客户端，会阻塞
                    send(new_socket, "ok", 2, 0);

                }
                close(new_socket);

            }

        }
        printf("connect fail");

    }

    close(server_socket);
    return 0;
}


