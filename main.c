#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>


#define SERVER_PORT 8080      //监听端口
#define BACKLOG     10        //最大连接数

int ok;

int main() {
    //创建socket，IPV4连接，启动TCP连接
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("create socket error\n");
        return -1;
    }

    //定义socket_addr
    struct sockaddr_in server_addr;     //指定服务器绑定地址
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    //绑定端口
    ok = bind(server_socket);
    if (ok == -1) {
        printf("bind error\n");
        return -1;
    }

    //设置监听信息
    ok = listen(server_socket, BACKLOG);
    if (ok == -1) {
        printf("listen error\n");
        return -1;
    }

    //阻塞监听请求，并创建业务线程
    while (true) {
        //accept();



    }













    return 0;
}