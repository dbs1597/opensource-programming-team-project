#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>


#define  BUFF_SIZE   1024
#define START 1
#define JOIN 2
void menu();
void movemenu();
void selm();
void gotoxy(int x, int y);
void prit_screen(char fname[]);
void initTermios(int echo);
void resetTermios(void);
char getch_(int echo);
char getch(void);

unsigned char key;
static struct termios old, new;
int menupos = 26;
int joinpos = 0 ;
char n,c;
char sel=NULL;
char joinid[10], joinpw[10], loginid[10], loginpw[10];
int joinchk = 1, loginchk=1, clearchk=0;
char msg[50];
   int   client_socket;
int servercon=0;

main (){

//   int   client_socket;

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
if(servercon==0)
  memset(msg,'1',1);
//char msg[]="star";   
//else if(servercon==1)
//char msg[]="join";
//memset(msg,'2',1);
write(client_socket,msg,sizeof(msg));




print_screen("maintest.txt");
while(1){
menu();
movemenu();

if(servercon==1)
//char msg[]="join";
memset(msg,'2',1);
write(client_socket,msg,sizeof(msg));


}



close( client_socket);
  
  

}

void initTermios(int echo){
tcgetattr(0, &old);
new = old;
new.c_lflag &= ~ICANON;
new.c_lflag &= echo ? ECHO : ~ECHO;
tcsetattr(0, TCSANOW , &new);
}

void resetTermios(void){
tcsetattr(0, TCSANOW, &old);
}

char getch_(int echo){
char ch;
initTermios(echo);
ch = getchar();
resetTermios();
return ch;
}

char getch(void){
return getch_(0);
}

void menu(){
gotoxy(26,60);
printf("  LOGIN\n");
gotoxy(28,60);
printf("  JOIN\n");
gotoxy(30,60);
printf("  EXIT\n");
gotoxy(menupos,60);
printf("▶ ");

}

void movemenu(){
int i;
c=getch();

if(c==27) //방향키 입력 3번들어오기때문에 27-91-방향키코드 순서로 받음 
{
   c = getch();
   if(c==91)
   {
      c= getch();
      if(c==66)
      {
         menupos=menupos+2;

         if(menupos>30){
            menupos=30;
          }
      }

      else if(c==65){
      menupos= menupos-2;

         if(menupos<26){
         menupos=26;
         }
      }
   }
}


else if(menupos==28&&c==10){ //select join
servercon=1;
print_screen2("join.txt");
gotoxy(18,58);
printf("\033[46m");
scanf("%s",&joinid); //나중에 파일로 저장해서 서버에서 다루기 
printf("\033[0m");
c=getch();


if(c==10){
gotoxy(20,58);
printf("\033[46m");
scanf("%s",&joinpw);
printf("\033[0m");
}
join();
}

else if(menupos==26&&c==10){
login();
}

}

void login(){
print_screen2("login.txt");
gotoxy(18,58);
printf("\033[46m");
scanf("%s",&loginid);
printf("\033[0m");

c=getch();
if(c==10){
gotoxy(20,58);
printf("\033[46m");
scanf("%s",&loginpw);
printf("\033[0m");
}

c= getch();


if(c==10){ //패스까지 입력후 엔터하면 로그인버튼으로 선택 
gotoxy(24,68);
printf("\033[36m\033[40mlogin\033[0m");
waitingroom();



while(1){
c=getch();

if(c==27){
c=getch();
if(c==91){
c=getch();
if(c==68||c==67){
  if(loginchk==1){ //취소 선택시 
  gotoxy(24,68);
  printf("\033[30m\033[46mlogin\033[0m");
  gotoxy(24,56);
  printf("\033[36m\033[40mexit\033[0m");
  loginchk=2;
  clearmain();
  }
  else{ //로그인 선택시 
  gotoxy(24,56);
  printf("\033[30m\033[46mexit\033[0m");
  gotoxy(24,68);
  printf("\033[36m\033[40mlogin\033[0m");
  loginchk=1;
  waitingroom();
  }
}
}
}

}//while  

}//enter 


}

void waitingroom(){
c=getch();
if(c==10);
system("clear");
printf("\n");
print_screen3("waitingroom.txt");
gotoxy(32,5);
c=getch();
if(c==96){
help();
}
else if(c==27){
c=getch();
if(c==79){
c=getch();
if(c==81);
start();
}//79
}//27

}//waitingroom

void start(){
system("clear");
printmap();
printna();
num();
print_ginfo("gameinfo.txt");
}

void num(){
gotoxy(8,33);
printf("┌─────────┐┌─────────┐\n");
gotoxy(9,33);
printf("│ ●    ●  ││	     │\n");
gotoxy(10,33);
printf("│	  ││    ●    │\n"); 
gotoxy(11,33);
printf("│ ●    ●  ││	     │\n");
gotoxy(12,33);
printf("└─────────┘└─────────┘\n");
gotoxy(13,33);
printf("   주사위 : ");
}

void help(){
print_screen4("help.txt");
}

void join(){

if(c==10){ //패스까지 입력후 엔터하면 조인버튼으로 선택 
joinpos=69;
gotoxy(24,joinpos);
printf("\033[36m\033[40mjoin\033[0m");

while(1){
c=getch();

if(c==27){
c=getch();
if(c==91){
c=getch();
if(c==68||c==67){
  if(joinchk==1){ //취소선택시 
  gotoxy(24,69);
  printf("\033[30m\033[46mjoin\033[0m");
  gotoxy(24,56);
  printf("\033[36m\033[40mexit\033[0m");
  joinchk=2;
  }
  else{ //가입하기 선택시 
  gotoxy(24,56);
  printf("\033[30m\033[46mexit\033[0m");
  gotoxy(24,69);
  printf("\033[36m\033[40mjoin\033[0m");
  joinchk=1;
  }
}//67||68
}//91
}//27
clearmain();
}//while
}

}


void clearmain(){
c=getch();
if(c==10){
system("clear");
printf("\n");
print_screen("maintest.txt");
 while(1){
 menu();
 movemenu();
 }
}
}

void gotoxy(int x, int y){
printf("\033[%d;%df",x,y);
fflush(stdout);
}

void print_screen2(char fname[]){
FILE *fp;
char line[100];
int i,j;
int joinpos=15;
if((fp=fopen(fname,"r")) == NULL)
{
printf("file open error");
getchar();
exit(-1);
}
for(i=0;i<9;i++){
joinpos=joinpos+1;
gotoxy(joinpos,50);
printf("\033[46m%s\033[0m",fgets(line,99,fp));
}
//while(1){
//if(fgets(line,99,fp)==NULL){
//break;
//}
//printf("%s",line);
//}
fclose(fp);
}

void print_screen(char fname[]){
FILE *fp;
char line[100];
int i;
if((fp=fopen(fname,"r")) == NULL)
{
printf("file open error");
getchar();
exit(-1);
}
//usleep(950000);
for(i=1;i<=5;i++){
usleep(30000);
printf("\033[31m%s\033[0m",fgets(line,99,fp));
}
for(i=6;i<=8;i++){
usleep(30000);
printf("\033[34m%s\033[0m",fgets(line,99,fp));
}
for(i=9;i<=10;i++){
usleep(30000);
printf("\033[35m%s\033[0m",fgets(line,99,fp));
}
for(i=11;i<=16;i++){
usleep(30000);
printf("\033[32m%s\033[0m",fgets(line,99,fp));
}
for(i=17;i<=22;i++){
usleep(30000);
printf("\033[33m%s\033[0m",fgets(line,99,fp));
}
for(i=23;i<=27;i++){
usleep(30000);
printf("\033[37m%s\033[0m",fgets(line,99,fp));
}
for(i=28;i<=30;i++){
usleep(30000);
printf("\033[36m%s",fgets(line,99,fp));
}
for(i=31;i<=sizeof(line);i++){
usleep(30000);
printf(fgets(line,99,fp));
}
printf("\033[0m");

fclose(fp);


}

void print_ginfo(char fname[]){
FILE *fp;
char line[100];
int i;
int ginfopos=4;
if((fp=fopen(fname,"r")) == NULL)
{
printf("file open error");
getchar();
exit(-1);
}

gotoxy(1,90);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));
gotoxy(2,90);
printf(fgets(line,99,fp));
gotoxy(3,90);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));
for(i=7;i<=25;i++){
gotoxy(ginfopos,90);
ginfopos=ginfopos+1;
printf(fgets(line,99,fp));
}
gotoxy(23,90);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));
for(i=24;i<=30;i++){
gotoxy(i,90);
printf(fgets(line,99,fp));
}
gotoxy(31,90);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));
gotoxy(32,90);
printf(fgets(line,99,fp));
gotoxy(33,90);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));



fclose(fp);
}

void print_screen3(char fname[]){
FILE *fp;
char line[100];
int i;
if((fp=fopen(fname,"r")) == NULL)
{
printf("file open error");
getchar();
exit(-1);
}

printf("\033[31m");
for(i=1;i<=16;i++){
printf(fgets(line,99,fp));
}
printf("\033[34m");
for(i=17;i<=25;i++){
printf(fgets(line,99,fp));
}
printf("\033[35m");
for(i=26;i<=36;i++){
printf(fgets(line,99,fp));
}
printf("\033[32m");
for(i=37;i<=47;i++){
printf(fgets(line,99,fp));
}
printf("\033[33m");
for(i=48;i<=59;i++){
printf(fgets(line,99,fp));
}
printf("\033[37m");
for(i=60;i<=70;i++){
printf(fgets(line,99,fp));
}
printf("\033[36m");
for(i=71;i<=sizeof(line);i++){
printf(fgets(line,99,fp));
}


fclose(fp);
printf("\033[0m");

}

void print_screen4(char fname[]){
FILE *fp;
char line[100];
int i,j;
int helppos=4;
if((fp=fopen(fname,"r")) == NULL)
{
printf("file open error");
getchar();
exit(-1);
}
printf("\033[46m");
gotoxy(3,39);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));
gotoxy(4,39);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));

for(i=4;i<=28;i++){
helppos=helppos+1;
gotoxy(helppos,39);
printf(fgets(line,99,fp));
//printf("\n");
}
gotoxy(30,39);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));
gotoxy(31,39);
printf(fgets(line,99,fp));
printf(fgets(line,99,fp));


printf("\033[0m");
fclose(fp);
}



void printmap(){
int i;
//gotoxy(0,0);
//printf("\n");
printf("\n┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐\n");
for(i=1;i<=3;i++)
{
printf("│	   │          │          │          │          │          │          │          │\n");
}
printf("├──────────┼──────────┴──────────┴──────────┴──────────┴──────────┴──────────┼──────────┤\n");
for(i=1;i<=5;i++){
printf("│          │                                                                 │          │\n");
printf("│	   │								     │	        │\n");
printf("│	   │								     │	        │\n");
printf("├──────────┤								     ├──────────┤\n");
}
for(i=1;i<=3;i++)
printf("│          │                                                                 │          │\n");

printf("├──────────┼──────────┬──────────┬──────────┬──────────┬──────────┬──────────┼──────────┤\n");
for(i=1;i<=3;i++)
printf("│          │          │          │          │          │          │          │          │\n");
printf("└──────────┴──────────┴──────────┴──────────┴──────────┴──────────┴──────────┴──────────┘\n");
}

void printna(){
int i;
gotoxy(4,4);
printf("무인도");
gotoxy(3,16);
printf("속초");
gotoxy(3,27);
printf("강릉");
gotoxy(3,38);
printf("강원");
gotoxy(3,49);
printf("평창");
gotoxy(3,58);
printf("사회복지");
gotoxy(4,60);
printf("기금");
gotoxy(5,59);
printf("접수처");
gotoxy(3,71);
printf("춘천");
gotoxy(4,80);
printf("황금열쇠");
gotoxy(7,82);
printf("인천");
gotoxy(11,82);
printf("파주");
gotoxy(15,82);
printf("용인");
gotoxy(19,82);
printf("가평");
gotoxy(23,82);
printf("서울");
gotoxy(27,82);
printf("제주");
gotoxy(32,82);
printf("시작");
gotoxy(31,71);
printf("안동");
gotoxy(31,60);
printf("전주");
gotoxy(31,49);
printf("여수");
gotoxy(31,38);
printf("독도");
gotoxy(31,27);
printf("경주");
gotoxy(31,16);
printf("부산");
gotoxy(32,3);
printf("황금열쇠");
gotoxy(27,5);
printf("대구");
gotoxy(23,5);
printf("대전");
gotoxy(20,3);
printf("황금열쇠");
gotoxy(15,5);
printf("단양");
gotoxy(11,3);
printf("사회복지");
gotoxy(12,5);
printf("기금");
gotoxy(13,4);
printf("수령처");
gotoxy(7,5);
printf("태안");
}
