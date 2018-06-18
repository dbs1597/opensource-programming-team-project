#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char msg;

#define ACCESS_ACCEPT 1
#define ACCESS_DENY 2
#define LOGIN_REQUEST 3
#define LOGIN_ACCEPT 4
#define LOGIN_DENY 5
#define JOIN_REQUEST 6
#define JOIN_ACCEPT 7
#define JOIN_DENY 8
#define CHAT_REQUEST 9
#define CREATE_ROOM_REQUEST 10
#define BREAK_ROOM_REQUEST 11
#define EXIT_ROOM_REQUEST 12

#define BASICSIZE 12
#define CHATBUFSIZE 58

struct member{
char id[BASICSIZE+1];
char pw[BASICSIZE+1];
char name[5];
}member;

struct room{
int roomnum;
char ip[16];
char roomname[CHATBUFSIZE+1];
char hostid[BASICSIZE+1];
char hostport[6];
}room;

struct loginmember{
char id[BASICSIZE+1];
char ip[16];
char name[5];
int sock;
}loginmember;

