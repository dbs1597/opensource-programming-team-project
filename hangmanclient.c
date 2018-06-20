#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_SIZE 50

void error_handling(char *message);
void wrt(void * arg);
void rcv(void * arg);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    
    if(argc!=3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    
    //socket
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
    
    //connect
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");
    puts("Connecting success!!!");
    
    //write
    wrt((void *) &sock);
    //close
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

void wrt(void * arg) {
    
    int sock = *((int *)arg);
    char input[BUF_SIZE];
    while(strcmp("quit", input ) != 0) {
        fgets(input, BUF_SIZE, stdin);
        write(sock, input, BUF_SIZE);
    }
}

void rcv(void * arg) {
    int sock = *((int*)arg);
    int rcv_cnt;
    char input[BUF_SIZE];
    
    while(1) {
        rcv_cnt = read(sock, &input, BUF_SIZE-1);
        if(rcv_cnt == -1) {
            error_handling("read() error!");
        }
        
        input[rcv_cnt-1] = 0;
        if(strcmp(input, "fail") == 0) {
            error_handling("error!");
        }
        
        write(sock, input, rcv_cnt);
    }
}
