#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define SERVER_PORT 8000      //监听端口
#define BUFF_SIZE   1024      //接收区长度
#define BACKLOG     10        //最大连接数

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
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //绑定端口
    ret = bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (ret == -1) {
        printf("bind error\n");
        close(server_socket);
        exit(-1);
    }

    //设置监听信息，通知内核启动监听，等待建立三次握手
    ret = listen(server_socket, BACKLOG);
    if (ret == -1) {
        printf("listen error\n");
        close(server_socket);
        exit(-1);
    }
    printf("server is listening on port %d\n", SERVER_PORT);
    system("netstat -an | grep 8000");	// 查看连接状态

    while (true) {
        int addr_len = sizeof(struct sockaddr);
        //从阻塞队列拿一个连接出来，没有则阻塞，并创建业务线程
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (new_socket != -1) {
            printf("connect success");
            client_num ++;
            printf("Get connect from client %d : %s\n",  client_num, inet_ntoa(client_addr.sin_addr));
            //fork会创建子线程，两次返回，写时复制
            if (!fork()) {
                /* 子线程代码 */
                while (true) {
                    int recv_len;                            //返回长度
                    unsigned char recv_buf[BUFF_SIZE];       //接收数据区

                    //接收数据，会阻塞
                    recv_len = recv(new_socket, recv_buf, BUFF_SIZE-1, 0);
                    if (recv_len == 0) {
                        shutdown(new_socket, SHUT_RDWR);        //四次断开，shutdown可以无视多线程引用
                        printf("client stop connection\n");
                        return 0;
                    }
                    if (recv_len < 0) {
                        printf("read client socket file error, try again\n");
                        sleep(2);
                        continue;
                    }
                    recv_buf[recv_len] = '\0';

                    //打印接收的字符串
                    printf("Get Msg From Client %d: %s\n", client_num, recv_buf);

                    //发送一些数据给客户端，会阻塞
                    send(new_socket, "ok", strlen("ok"), 0);

                }

            }

        }
        printf("connect fail");

    }

}


