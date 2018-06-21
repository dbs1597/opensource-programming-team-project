//
//  main.c
//  client
//
//  Created by knuprime082 on 2017. 11. 2..
//  Copyright © 2017년 knuprime082. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define IP_ADR "192.168.43.175"
#define PORT 9999

void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(IP_ADR);
    serv_addr.sin_port=htons(PORT);
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");
    
    message[0] = 0;
    while(strcmp(message , "exit") != 0)
    {
        printf("Enter a message : ");
        fgets(message , sizeof(message) , stdin);
        write(sock, message, strlen(message));
    }
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}//가나다라마바사
