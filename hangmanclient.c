#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message);
void wrt(void * arg);

int main(int argc, char* argv[])
{
    int sock, size, fork_ret;
    char line[BUF_SIZE], sendline[BUF_SIZE], recvline[BUF_SIZE+1];
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
    fork_ret = fork();
    if(fork_ret > 0)
    {
        // 부모 프로세스는 키보드 입력을 서버로 송신
        while(fgets(sendline, BUF_SIZE, stdin) != NULL)
        {
            size = strlen(sendline);
            if(write(sock, sendline, strlen(sendline)) != size)
            {
                printf("Error in write. \n");
            }
            if(strstr(sendline, "quit") != NULL) // 종료 문자열
            {
                printf("Good byte.\n");
                close(sock);
                while(1);    //자식프로세서가 죽을때까지 블로킹
            }
        }
    }
    else if(fork_ret == 0)
    {
        // 자식 프로세스는 서버로부터 수신된 메시지를 화면에 출력
        while(1)
        {
            if((size = read(sock, recvline, BUF_SIZE)) < 0)
            {
                printf("Error if read. \n");
                close(sock);
                return 0;
            }
            recvline[size] = '\0';
            if(strstr(recvline, "quit")!=NULL) // 종료 문자열
            {
                write(sock, "quit", strlen("quit"));
                break;
            }
            printf("%s", recvline); // 화면 출력
        }
    }
    
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
