#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLEN 1024

void error_handling(char *message);
char* drawHangman(int num);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    int read_len , idx;
    int i, j, iscorrect, chance, last;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    
    char word[MAXLEN], question[MAXLEN];
    
    char message[MAXLEN];
    
    if(argc!=2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));
    
    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error");
    
    if(listen(serv_sock, 5)==-1)
        error_handling("listen() error");
    
    clnt_addr_size=sizeof(clnt_addr);
    
    printf("정답단어를 입력하세요.\n");
    scanf("%s", word);
    
    for(i=0;  i<strlen(word); i++)
        question[i]='*';
    question[i]='\0';
    
    iscorrect = 0;
    chance = 0;
    last = 0;
    
    while(1)
    {
        puts("Server] Listening...");
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
        if(clnt_sock==-1)
            error_handling("accept() error");
        puts("Server] Accepted!!");
        
        send(clnt_sock, "행맨게임에 오신 것을 환영합니다.\n", strlen("행맨게임에 오신 것을 환영합니다.\n"), 0);
        send(clnt_sock, "소문자 알파벳을 입력하세요.\n", strlen("소문자 알파벳을 입력하세요.\n"), 0);
        
        message[0] = 0;
        idx = 0;
        while(read(clnt_sock, &message[0], 1) == 1)
        {
            
            if(read_len==-1){
                error_handling("read() error!");
                exit(1);
            }
            printf("1 %d\n", strlen(message));
            
            if(strlen(message)==1) {
                printf("2\n");
                for(i=0;  i<strlen(word); i++){
                    if(word[i]==message[0] && question[i] != message[0]){
                        iscorrect++;
                        question[i]=word[i];
                        if (iscorrect == strlen(word)) {
                            send(clnt_sock, word, strlen(word), 0);
                            send(clnt_sock, "단어완성!\n", strlen("단어완성!\n"), 0);
                            close(clnt_sock);
                            close(serv_sock);
                            return 0;
                        }
                    }
                }
                if (iscorrect == last)
                    chance++;
                
                if (chance == 7) {
                    //drawHangman(7);
                    send(clnt_sock, drawHangman(7), strlen(drawHangman(7)), 0);
                    send(clnt_sock, "행맨이 죽었습니다ㅜ\n", strlen("행맨이 죽었습니다ㅜ\n"), 0);
                    send(clnt_sock, "정답은 ", strlen("정답은 "), 0);
                    send(clnt_sock, word, strlen(word), 0);
                    send(clnt_sock, "입니다\n", strlen("입니다\n"), 0);
                    close(clnt_sock);
                    close(serv_sock);
                    return 0;
                }
                
                send(clnt_sock, drawHangman(chance), strlen(drawHangman(chance)), 0);
                send(clnt_sock, "단어 ", strlen("단어 "), 0);
                send(clnt_sock, question, strlen(question), 0);
                send(clnt_sock, " 소문자 알파벳을 입력하세요.\n", strlen(" 소문자 알파벳을 입력하세요.\n"), 0);
                message[0] = 0;
                last = iscorrect;
                
                
                
                //            if(message[idx++] == '\n'){
                //                message[idx] = 0;
                //
                //                //-----------------------write code to do---------------------------------
                //                printf("Message from Client: %s", message);
                //                //------------------------------------------------------------------------
                //                message[0] = 0 ;
                //                idx = 0;
                //            }
            }
        }
        
        sleep(1);
    }
    
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

char* drawHangman(int num){
    char* hangman={ 0 };
    switch (num)
    {
        case 0:
            hangman = "┌───┐\n│\n│\n│\n│\n└──────\n";
            break;
        case 1:
            hangman = "┌───┐\n│　○\n│\n│\n│\n└──────\n";
            break;
        case 2:
            hangman = "┌───┐\n│　○\n│　 |\n│\n│\n└──────\n";
            break;
        case 3:
            hangman = "┌───┐\n│　○\n│　/|\n│\n│\n└──────\n";
            break;
        case 4:
            hangman = "┌───┐\n│　○\n│　/|＼\n│　\n│\n└──────\n";
            break;
        case 5:
            hangman = "┌───┐\n│　○\n│　/|＼\n│　/\n│\n└──────\n";
            break;
        case 6:
            hangman = "┌───┐\n│　○\n│　/|＼\n│　/＼\n│\n└──────\n";
            break;
        case 7:
            hangman = "┌───┐\n│　○\n│　 X\n│　/|＼\n│　/＼\n└──────\n";
            break;
        default:
            hangman = "drawing error\n";
            break;
    }
    return hangman;
}

