#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


#define  BUFF_SIZE   1024

int   main( int argc, char **argv)
{
   int   client_socket;

   struct sockaddr_in   server_addr;

   char   buff[BUFF_SIZE+5];

   client_socket  = socket( PF_INET, SOCK_STREAM, 0);
   if( -1 == client_socket)
   {
      printf( "socket 생성 실패n");
      exit( 1);
   }

   memset( &server_addr, 0, sizeof( server_addr));
   server_addr.sin_family     = AF_INET;
   server_addr.sin_port       = htons( 4000);
   server_addr.sin_addr.s_addr= inet_addr( "192.168.78.130");

   if( -1 == connect( client_socket, (struct sockaddr*)&server_addr, sizeof( server_addr) ) )
   {
      printf( "접속 실패n");
      exit( 1);
   }
int num;
char msg[50];
char id[10];
char pw[10];
#define JOIN 1
#define LOGIN 2
printf("===menu===\n");
printf("1.join\n");
printf("2.login\n");
printf("select >> ");
scanf("%s",msg);

if(strcmp(msg,"1")==0){
printf("join");
}
else if(strcmp(msg,"2")==0){
printf("id : ");
scanf("%s",&id);
printf("pw : ");
scanf("%s",&pw);
//write(client_socket,id,sizeof(id));
//write(client_socket, pw,sizeof(pw));
if(strcmp(id,"test")==0 && strcmp(pw,"test")==0)
printf("next\n");
}
write(client_socket,msg,sizeof(msg));
//   write( client_socket, argv[1], strlen( argv[1])+1);      // +1: NULL까지 포함해서 전송
//   read ( client_socket, buff, BUFF_SIZE);
//   printf( "%s\n", buff);
   close( client_socket);
  
   return 0;
}
