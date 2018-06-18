#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "connect.h"
#define  BUFF_SIZE   1024

int   main( void)
{
   int   server_socket;
   int   client_socket;
   int   client_addr_size;

   struct sockaddr_in   server_addr;
   struct sockaddr_in   client_addr;

   char   buff_rcv[BUFF_SIZE+5];
   char   buff_snd[BUFF_SIZE+5];



   server_socket  = socket( PF_INET, SOCK_STREAM, 0);
   if( -1 == server_socket)
   {
      printf( "server socket 생성 실패n");
      exit( 1);
   }

   memset( &server_addr, 0, sizeof( server_addr));
   server_addr.sin_family     = AF_INET;
   server_addr.sin_port       = htons( 4000);
   server_addr.sin_addr.s_addr= htonl( INADDR_ANY);

   if( -1 == bind( server_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
   {
      printf( "bind() 실행 에러n");
      exit( 1);
   }

   while( 1)
   {
      if( -1 == listen(server_socket, 5))
      {
         printf( "대기상태 모드 설정 실패n");
         exit( 1);
      }

      client_addr_size  = sizeof( client_addr);
      client_socket     = accept( server_socket, (struct sockaddr*)&client_addr, &client_addr_size);

      if ( -1 == client_socket)
      {
         printf( "클라이언트 연결 수락 실패n");
         exit( 1);
      }
//      _beginthread(recv_client,0,&client_socket);
      char msg[BUFF_SIZE+5];

      read ( client_socket, msg, BUFF_SIZE);
      
      //if(msg == 1)
      //{printf("select join\n");}
      //else if(msg ==2){
      //printf("select login\n");
     // }
      printf( "receive: %s\n", msg);
      
     // sprintf( buff_snd, ">> %s", buff_rcv);
     // write( client_socket, buff_snd, strlen( buff_snd)+1);          // +1: NULL까지 포함해서 전송
      close( client_socket);
   }

   

}
/*
void recv_client(void *sock){

int ret,i;
int mysock=*(SOCKET*)sock;

msg=ACCESS_ACCEPT;
ret=send(*(SOCKET*)sock, &msg, 1,0);

//while(ret!=SOCKET_ERROR || ret!=INVALID_SOCKET){
msg=0;
ret=recv(*(SOCKET*)sock, &msg,1,0);

if(msg!=0)
printf("%d requers ",msg);

if(msg==LOGIN_REQUEST){
printf("login");
}

if(msg==JOIN_REQUEST)
printf("join");


//}

close(client_socket);

}
*/
