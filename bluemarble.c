#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // time
#include <unistd.h> // Sleep, 사용자 함수 kbhit 구현
#include <termio.h> // 사용자 함수 getch 구현
#include <termios.h> // 사용자 함수 kbhit 구현
#include <fcntl.h> // 사용자 함수 kbhit 구현

#define BOARD 28  // 보드의 칸의 갯수
#define PLAYER 2 // 플레이어 수

// 키보드 상수 값 설정
#define SPACE 32; // 키보드 상수 값
#define ENTER 13;
#define UP 72;
#define DOWN 80;

#define DEFAULT printf("%c[%dm", 0x1B, 0)
#define WHITE printf("\x1b[37m") // YES or NO
#define YELLOW printf("\x1b[33m") // 플레이어1
#define MAGENTA printf("\x1b[35m") // 화살표
#define RED printf("\x1b[31m") // 플레이어2
#define BLUE printf("\x1b[36m") // 공지
#define GREEN printf("\x1b[32m") // 주사위
#define BLACK printf("\x1b[30m")



void printmap(); // 맵을 출력해주는 함수
void printna(); // 맵 위에 도시이름을 출력해주는 함수
int dice_cast(); // 주사위 숫자 생성해주는 함수
void dice_print(); // dice_cast()로 생성한 주사위 눈의 수에 따른 주사위 모양 출력
void gotoxy(int x, int y); // 좌표 설정 함수
int getch(void); // getch
void insert_playername(int a); // 시작 시 플레이어 이름 입력하는 함수
void finish_bonus(int a); // 시작점에 도착해서 완주 시 완주보너스를 지급하는 함수
void player_state(); // 게임 시작 시 사용자 정보 초기화한걸 출력해주는 함수
void icon_print(int a); // 플레이어의 말이 위치한 말판 위에 플레이어의 말을 출력만 해주는 함수
void development_mark(int a); // 도시의 발전도를 표시하는 함수 / 구매전 0, 도시구매후 1, 주택건설후 2, 빌라건설후 3, 아파트건설 후 4
void clear(); // 플레이어들의 상태 정보, 주사위, 공지를 지우는 함수
void icon_movement(int a); // 주사위를 굴리고(dice_cast, dice_print), 말판위에 말 움직이는(icon_movement) 함수
void gold_key(int a); // 황금열쇠 함수
void trade(int a); // 사용자의 위치에 따른 행동을 하는 함수
// 사회복지기금 접수, 사회복지기금 수령 / 무인도(island) / 도시구매 / 주택, 빌라, 아파트 건설 / 통행료 납부 거래 / 인수(mna) 등
void player_state_update(); // 플레이어의 잔고, 보유 건물 수를 턴이 바뀔 때마다 업데이트하는 함수
void bankruptcy(int a); // 파산 검사 후 도시 매각 함수 (=도시의 소유주, 발전도 초기화)
void mna(int a); // 도시 인수를 하고, 도시의 새로운 소유주의 색으로 발전도를 출력하는(development_mark) 함수
void island(int a); // 무인도 함수
int yes_or_no(); // 도시 구입, 건물 건설, mna, 도시매각 시 사용자의 의사를 묻는 함수
void ending(int a); // 게임종료 조건이 달성되면(파산) 게임을 끝내고 승리자의 이름과 재산, 보유한 도시 수를 출력해주는 함수



typedef struct { // 플레이어 정보 구조체
char name[20]; // 플레이어의 이름
int money; // 플레이어의 소지금
int rest; // 무인도로 인한 휴식 처음에 0을 주고 황금열쇠에서 무인도에 걸리면 2를 주는거
int position; // 플레이어의 위치(몇번째 도시에 해당하는가)
char icon[4]; // 플레이어의 아이콘
}Player;



Player player[PLAYER] = { // 플레이어의 정보 초기화 // 여기부분도 수정
//  이름   소지금 무인도 위치 아이콘
{"플레이어1", 200, 0, 0, "★"}, // 아이콘 색 노란색 세우는 건물도 노란색
{"플레이어2", 200, 0, 0, "♥"} // 아이콘 색 빨간색 세우는 건물도 빨간색
}; // 플레이어의 정보 초기화



typedef struct { // 도시 정보 구조체 선언
char name[100]; // 도시이름
int development[4]; // 도시에 건물 값 , 0 도시구매가, 1 주택, 2 빌라, 3 아파트
int development_level; // 맨처음에 도시의 발전도 0 구매 -> 1 빌라 -> 2면 아파트 세울 수 있음
char owner[20]; // 도시 소유주 이름
int x[2]; // 플레이어1,2의 말의 x좌표
int y[2]; // 플레이어1,2의 말의 y좌표 // 플레이어 1과 2가 어느곳에 말을 두게 될지
int building_x; // 건물 x좌표
int building_y; // 건물 y좌표 // 플레이어가 세우는 건물이 어느위치에 생기는지
}City;



City city[BOARD] = {
// 도시 24개
// 도시이름, 가격[4], 발전도, 소유주, 플x좌표[2], 플y좌표[2], 도x좌표, 도y좌표
{"시작", {0, 0, 0, 0}, 0, "없음", {79, 87}, {33, 33}, 79, 31}, // 0 시작
{"제주", {50, 13, 16, 20}, 0, "없음", {79, 87}, {29, 29}, 79, 27}, // 1 제주
{"서울", {80, 29, 36, 43}, 0, "없음", {79, 87}, {25, 25}, 79, 23}, // 2 서울
{"가평", {10, 4, 6, 8}, 0, "없음", {79, 87}, {21, 21}, 79, 19}, // 3 가평
{"용인", {60, 17, 22, 30}, 0, "없음", {79, 87}, {17, 17}, 79, 15}, // 4 용인
{"파주", {8, 1, 2, 3}, 0, "없음", {79, 87}, {13, 13}, 79, 11}, // 5 파주
{"인천", {24, 6, 8, 10}, 0, "없음", {79, 87}, {9, 9}, 79, 7}, // 6 인천
{"황금열쇠", {0, 0, 0, 0}, 0, "없음", {79, 87}, {5, 5}, 79, 3}, // 7 황금열쇠
{"춘천", {30, 6, 8, 10}, 0, "없음", {68, 76}, {5, 5}, 68, 3}, // 8 춘천
{"사회복지기금 접수처", {0, 0, 0, 0}, 0, "없음", {57, 65}, {5, 5}, 57, 3}, // 9 사회복지기금 접수처
{"평창", {37, 15, 13, 9}, 0, "없음", {46, 54}, {5, 5}, 46, 3}, // 10 평창
{"강원", {15, 4, 6, 8}, 0, "없음", {35, 43}, {5, 5}, 35, 3}, // 11 강원
{"강릉", {11, 4, 6, 8}, 0, "없음", {24, 32}, {5, 5}, 24, 3}, // 12 강릉
{"속초", {29, 6, 8, 10}, 0, "없음", {13, 21}, {5, 5}, 13, 3}, // 13 속초
{"황금열쇠", {0, 0, 0, 0}, 0, "없음", {2, 10}, {5, 5}, 2, 3}, // 14 황금열쇠
{"태안", {22, 6, 8, 10}, 0, "없음", {2, 10}, {9, 9}, 2, 7}, // 15 태안
{"사회복지기금 수령처", {0, 0, 0, 0}, 0, "없음", {2, 10}, {13, 13}, 2, 11}, // 16 사회복지기금 수령처
{"단양", {5, 1, 2, 3}, 0, "없음", {2, 10}, {17, 17}, 2, 15}, // 17 단양
{"황금열쇠", {0, 0, 0, 0}, 0, "없음", {2, 10}, {21, 21}, 2, 19}, // 18 황금열쇠
{"대전", {51, 17, 22, 30}, 0, "없음", {2, 10}, {25, 25}, 2, 23}, // 19 대전
{"대구", {40, 9, 13, 15}, 0, "없음", {2, 10}, {27, 27}, 2, 27}, // 20 대구
{"무인도", {0, 0, 0, 0}, 0, "없음", {2, 10}, {33, 33}, 2, 31}, // 21 무인도///
{"부산", {80, 29, 36, 43}, 0, "없음", {13, 21}, {33, 33}, 13, 31}, // 22 부산
{"경주", {23, 6, 8, 10}, 0, "없음", {24, 32}, {33, 33}, 24, 31}, // 23 경주 
{"독도", {200, 0, 0, 0}, 0, "없음", {35, 43}, {33, 33}, 35, 31}, // 24 독도는 건물을 세울 수 없음
{"여수", {28, 6, 8, 10}, 0, "없음", {46, 54}, {33, 33}, 46, 31}, // 25 여수
{"전주", {55, 17, 22, 30}, 0, "없음", {57, 65}, {33, 33}, 57, 31}, // 26 전주
{"안동", {38, 9, 13, 15}, 0, "없음", {68, 76}, {33, 33}, 68, 31} // 27 안동
};



int welfare = 50; // 사회복지기금



int dice_num1_info; // 주사위 첫번째 눈 저장
int dice_num2_info; // 주사위 두번째 눈 저장



void printmap(){ // 맵을 출력해주는 함수
int i;
gotoxy(0,0);
printf("\n");
printf("┌──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┬──────────┐\n");
printf("│	   │          │          │          │          │          │          │          │\n");
printf("│	   │          │          │          │          │          │          │          │\n");
printf("│	   │          │          │          │          │          │          │          │\n");
printf("├──────────┼──────────┴──────────┴──────────┼──────────┴──────────┴──────────┼──────────┤\n");
printf("│          │                                │                                │          │\n");
printf("│	   │				    │		          	     │	        │\n");
printf("│	   │				    │		          	     │	        │\n");
printf("├──────────┤				    │		          	     ├──────────┤\n");
printf("│          │				    │		          	     │          │\n");
printf("│	   ├────────────────────────────────┴────────────────────────────────┤	        │\n");
printf("│	   │								     │	        │\n");
printf("├──────────┤								     ├──────────┤\n");
printf("│          │                                                                 │          │\n");
printf("│	   │								     │	        │\n");
printf("│	   │								     │	        │\n");
printf("├──────────┤								     ├──────────┤\n");
printf("│          │                                                                 │          │\n");
printf("│	   │								     │	        │\n");
printf("│	   │								     │	        │\n");
printf("├──────────┤								     ├──────────┤\n");
printf("│          │                                                                 │          │\n");
printf("│	   ├─────────────────────────────────────────────────────────────────┤	        │\n");
printf("│	   │                                                                 │	        │\n");
printf("├──────────┤                                                                 ├──────────┤\n");
printf("│          │                                                                 │	        │\n");
printf("│          │                                                                 │	        │\n");
printf("│          │                                                                 │	        │\n");
printf("├──────────┼──────────┬──────────┬──────────┬──────────┬──────────┬──────────┼──────────┤\n");
printf("│	   │          │          │          │          │          │          │          │\n");
printf("│	   │          │          │          │          │          │          │          │\n");
printf("│	   │          │          │          │          │          │          │          │\n");
printf("└──────────┴──────────┴──────────┴──────────┴──────────┴──────────┴──────────┴──────────┘\n");
} // void printfmap()



void printna() {
int i;
gotoxy(3,4);
printf("황금열쇠");
gotoxy(16,4);
printf("속초");
gotoxy(27,4);
printf("강릉");
gotoxy(38,4);
printf("강원");
gotoxy(49,4);
printf("평창");
gotoxy(58,3);
printf("사회복지");
gotoxy(60,4);
printf("기금");
gotoxy(59,5);
printf("접수처");
gotoxy(71,4);
printf("춘천");
gotoxy(80,4);
printf("황금열쇠");
gotoxy(82,8);
printf("인천");
gotoxy(82,12);
printf("파주");
gotoxy(82,16);
printf("용인");
gotoxy(82,20);
printf("가평");
gotoxy(82,24);
printf("서울");
gotoxy(82,28);
printf("제주");
gotoxy(82,32);
printf("시작");
gotoxy(71,32);
printf("안동");
gotoxy(60,32);
printf("전주");
gotoxy(49,32);
printf("여수");
gotoxy(38,32);
printf("독도");
gotoxy(27,32);
printf("경주");
gotoxy(16,32);
printf("부산");
gotoxy(4,32);
printf("무인도");
gotoxy(5,28);
printf("대구");
gotoxy(5,24);
printf("대전");
gotoxy(3,20);
printf("황금열쇠");
gotoxy(5,16);
printf("단양");
gotoxy(3,11);
printf("사회복지");
gotoxy(5,12);
printf("기금");
gotoxy(4,13);
printf("수령처");
gotoxy(5,8);
printf("태안");
}



int dice_cast() { // 주사위 숫자 생성해주는 함수
int dice_num;

srand(time(NULL)); // 시간에 따라 변화하는 랜덤 함수
dice_num = rand()%6 + 1; // 1~6 사이 난수 생성

return dice_num;
} // int dice_case()



void dice_print() { // 주사위 눈의 수에 따른 주사위 모양 출력
int dice_num1 = 0;
int dice_num2 = 0;

dice_num1 = dice_cast(); // 주사위 첫번째 숫자

if(dice_num1 == 1) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│	   │\n");
gotoxy(34,17);
printf("│   ●     │\n");
gotoxy(34,18);
printf("│         │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // if(dice_num1 == 1)

else if(dice_num1 == 2) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●	   │\n");
gotoxy(34,17);
printf("│	   │\n");
gotoxy(34,18);
printf("│     ●   │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 2)

else if(dice_num1 == 3) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●	   │\n");
gotoxy(34,17);
printf("│   ●     │\n");
gotoxy(34,18);
printf("│     ●   │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 3)

else if(dice_num1 == 4) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●    ●  │\n");
gotoxy(34,17);
printf("│	  │\n");
gotoxy(34,18);
printf("│ ●    ●  │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 4)

else if(dice_num1 == 5) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●    ●  │\n");
gotoxy(34,17);
printf("│    ●    │\n");
gotoxy(34,18);
printf("│ ●    ●  │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 5)

else if(dice_num1 == 6) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●    ●  │\n");
gotoxy(34,17);
printf("│ ●    ●  │\n");
gotoxy(34,18);
printf("│ ●    ●  │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 6)


sleep(1);
dice_num2 = dice_cast(); // 주사위 두번째 숫자

if(dice_num2 == 1) {
gotoxy(45,15);
printf("┌─────────┐");
gotoxy(45,16);
printf("│         │\n");
gotoxy(45,17);
printf("│   ●     │\n");
gotoxy(45,18);
printf("│         │\n");
gotoxy(45,19);
printf("└─────────┘\n");
} // if(dice_num2 == 1)

else if(dice_num2 == 2) {
gotoxy(45,15);
printf("┌─────────┐");
gotoxy(45,16);
printf("│ ●       │\n");
gotoxy(45,17);
printf("│         │\n");
gotoxy(45,18);
printf("│     ●   │\n");
gotoxy(45,19);
printf("└─────────┘\n");
} // else if(dice_num2 == 2)

else if(dice_num2 == 3) {
gotoxy(45,15);
printf("┌─────────┐");
gotoxy(45,16);
printf("│ ●	      │\n");
gotoxy(45,17);
printf("│   ●     │\n");
gotoxy(45,18);
printf("│     ●   │\n");
gotoxy(45,19);
printf("└─────────┘\n");
} // else if(dice_num2 == 3)

else if(dice_num2 == 4) {
gotoxy(45,15);
printf("┌─────────┐");
gotoxy(45,16);
printf("│ ●    ●  │\n");
gotoxy(45,17);
printf("│         │\n");
gotoxy(45,18);
printf("│ ●    ●  │\n");
gotoxy(45,19);
printf("└─────────┘\n");
} // else if(dice_num2 == 4)

else if(dice_num2 == 5) {
gotoxy(45,15);
printf("┌─────────┐");
gotoxy(45,16);
printf("│ ●    ●  │\n");
gotoxy(45,17);
printf("│    ●    │\n");
gotoxy(45,18);
printf("│ ●    ●  │\n");
gotoxy(45,19);
printf("└─────────┘\n");
} // else if(dice_num2 == 5)

else if(dice_num2 == 6) {
gotoxy(45,15);
printf("┌─────────┐");
gotoxy(45,16);
printf("│ ●    ●  │\n");
gotoxy(45,17);
printf("│ ●    ●  │\n");
gotoxy(45,18);
printf("│ ●    ●  │\n");
gotoxy(45,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 6)


dice_num1_info = dice_num1;
dice_num2_info = dice_num2;

gotoxy(15, 27);
printf("주사위를 던져서 [%d], [%d]가 나왔습니다.\n", dice_num1_info, dice_num2_info);
} // dice_print()



void gotoxy(int x, int y) { // 좌표 설정 함수
printf("\033[%d;%df",y,x);
fflush(stdout);
} // void gotoxy(int x, int y)



int getch(void) { // getch
int ch;
struct termios buf;
struct termios save;

tcgetattr(0, &save);
buf = save;
buf.c_lflag &= ~(ICANON | ECHO);
buf.c_cc[VMIN] = 1;
buf.c_cc[VTIME] = 0;
tcsetattr(0, TCSAFLUSH, &buf);
ch = getchar();
tcsetattr(0, TCSAFLUSH, &save);

return ch;
} // int getch(void)



void insert_playername(int a) { // 시작 시 플레이어 이름 입력
gotoxy(15, 25);
printf("플레이어%d 이름 입력 : ", 1);
scanf("%s", player[0].name);

gotoxy(15, 26);
printf("플레이어%d 이름 입력 : ", 2);
scanf("%s", player[1].name);
} // void insert_playername



void finish_bonus(int a) { // 시작점에 도착해서 완주 시
gotoxy(15,25);
printf("                                                         \n");
gotoxy(15,26);
printf("                                                         \n");
gotoxy(15,27);
printf("                                                         \n");
gotoxy(15,28);
printf("                                                         \n");

gotoxy(15,25);
BLUE; // 공지 파란색으로 뜸
printf("[%s]님 완주!\n", player[a].name); // 플레이어의 이름 출력
// gotoxy(공지 뜨는 위치 y축 한칸 아래)
gotoxy(15,26);
printf("완주 보너스 10만원 지급!\n");
player[a].money += 10; // 소지금 + 10
} // void finish_bonus(int a)



void player_state() { // 게임 시작 시 셋팅
//for (i=0 ; i<PLAYER ; i++) {
//if(i==0) { // 플레이어 1이면
YELLOW; // 노란색으로 출력
gotoxy(15,7);
printf("[%s]님 정보\n", player[0].name);
gotoxy(15,8);
printf("소지금 : [%d만원]\n", player[0].money);
gotoxy(15,9);
printf("소유 도시 갯수 : [0]개\n");
//}

//if(i==1) {
RED;
gotoxy(48,7);
printf("[%s]님 정보\n", player[1].name);
gotoxy(48,8);
printf("소지금 : [%d만원]\n", player[1].money);
gotoxy(48,9);
printf("소유 도시 갯수 : 0개\n");
//}

/*
int j; // 도시 돌림
int city_count = 0;
for(j=0 ; j<BOARD ; j++) {
if(i == 0) { // 플레이어 1의 소유 도시 수 세기
if(!strcmp(city[j].owner, player[0].name))
city_count++;
} // if(a==0)

else {
if(!strcmp(city[j].owner, player[1].name))
city_count++;
}
} // for BOARD의 수 만큼 for문 돌림
printf("소유 도시 갯수 : %d개\n", city_count);*/
//} // for 플레이어 수
} // void player_state();



void icon_print(int a) { // 플레이어의 말이 위치한 말판 위에 플레이어의 말을 출력하는 함수
gotoxy(city[player[a].position].x[a], city[player[a].position].y[a]);
// gotoxy(플레이어가 위치한 몇번째 말판.x좌표, 플레이어가 위치한 몇번째 말판.y좌표)

if(a==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(a==1) // 플레이어 2이면
RED; // 빨간색으로 출력

printf("%s\n", player[a].icon);
} // void icon_print(int a)



void development_mark(int a) { // 건물 구입 시 표시하는 함수 (= 발전도)
gotoxy(city[player[a].position].building_x, city[player[a].position].building_y);
// gotoxy(플레이어의 말이 위치한 몇번째 발판.건물표시 x위치, 플레이어의 말이 위치한 몇번째 발판.건물표시 y위치)

int i;
for(i=0 ; i<=city[player[a].position].development_level ; i++) { // 플레이어의 말이 있는 도시의 발전도만큼 반복
if(a==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(a==1) // 플레이어 2이면
RED; // 빨간색으로 출력

printf("█ "); // 플레이어의 말이 있는 도시의 발전도 만큼 건물아이콘을 출력함
} //for
printf("\n");
} // void development_mark(int a)


// 테스트 완료
void clear() { // 글자 지우기
// 공지의 글씨를 지웁니다
gotoxy(15,25);
printf("                                                         \n");
gotoxy(15,26);
printf("                                                         \n");
gotoxy(15,27);
printf("                                                         \n");
gotoxy(15,28);
printf("                                                         \n");

// 주사위도 지웁시다
gotoxy(34,15);
printf("                               ");
gotoxy(34,16);
printf("                               ");
gotoxy(34,17);
printf("                               ");
gotoxy(34,18);
printf("                               ");
gotoxy(34,19);
printf("                               ");

// 사용자 정보도 지웁시더
gotoxy(15,7);
printf("                   ");
gotoxy(15,8);
printf("                   ");
gotoxy(15,9);
printf("                   ");

gotoxy(48,7);
printf("                   ");
gotoxy(48,8);
printf("                   ");
gotoxy(48,9);
printf("                   ");
} // void clear()



void icon_movement(int a) { // 주사위 굴리고, 말판위에 말 움직이는 함수
//int d; // 주사위

if(a==0) // 플레이어 1이면
YELLOW; // 노란색으로 출력
if(a==1) // 플레이어 2이면
RED; // 빨간색으로 출력

gotoxy(15,25);
printf("[%s]님의 차례입니다.\n", player[a].name);
gotoxy(15,26);
printf("주사위를 굴려주세요.\n");
getch();
sleep(1);
GREEN; // 주사위 색깔
dice_print(); // 주사위 굴리고 뭐가 나왔는지 시각적으로 보여주고
gotoxy(15,27);
printf("[%d], [%d]이 나왔습니다.",dice_num1_info, dice_num2_info); // 공지창에다가 출력

int dice_num_sum;
dice_num_sum = dice_num1_info + dice_num2_info;

int i;
for(i=0 ; i < dice_num_sum ;  i++) {
gotoxy(city[player[a].position].x[a], city[player[a].position].y[a]); // 플레이어의 말이 현재 위치한 곳에 가서
printf("  "); // 말 지나간 자리 지우기

if(player[a].position > 25) { // 플레이어가 마지막 말판에 도착하게 되면
player[a].position = 0; // 시작 위치로 돌아오고
finish_bonus(a); // 완주 보너스 지급
sleep(2);
icon_print(a); // 플레이어의 말 출력
}//if

else {
player[a].position += 1;
icon_print(a);
} // else
} // for
} // void icon_movement(int a)



void gold_key(int a) {
int d; // 주사위

BLUE;
gotoxy(15,25);
printf("[황금열쇠]칸에 도착하였습니다.\n");
gotoxy(15,26);
printf("주사위를 굴려주세요.\n");

getch(); // 키보드가 입력되면
gotoxy(34,15);
printf("                               ");
gotoxy(34,16);
printf("                               ");
gotoxy(34,17);
printf("                               ");
gotoxy(34,18);
printf("                               ");
gotoxy(34,19);
printf("                               ");

d = dice_cast();
sleep(1); // 1초 후에
GREEN;
if(d == 1) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│	  │\n");
gotoxy(34,17);
printf("│   ●     │\n");
gotoxy(34,18);
printf("│         │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // if(dice_num1 == 1)

else if(d == 2) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●	  │\n");
gotoxy(34,17);
printf("│	  │\n");
gotoxy(34,18);
printf("│     ●   │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 2)

else if(d == 3) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●	  │\n");
gotoxy(34,17);
printf("│   ●     │\n");
gotoxy(34,18);
printf("│     ●   │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 3)

else if(d == 4) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●    ●  │\n");
gotoxy(34,17);
printf("│	  │\n");
gotoxy(34,18);
printf("│ ●    ●  │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 4)

else if(d == 5) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●    ●  │\n");
gotoxy(34,17);
printf("│    ●    │\n");
gotoxy(34,18);
printf("│ ●    ●  │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 5)

else if(d == 6) {
gotoxy(34,15);
printf("┌─────────┐");
gotoxy(34,16);
printf("│ ●    ●  │\n");
gotoxy(34,17);
printf("│ ●    ●  │\n");
gotoxy(34,18);
printf("│ ●    ●  │\n");
gotoxy(34,19);
printf("└─────────┘\n");
} // else if(dice_num1 == 6)

gotoxy(15,25);
printf("[%d]가 나왔습니다.\n", d);

switch(d) { // 주사위값
case 1: // 1. 사회복지기금 수령처로 이동 (플레이어 소지금액 + 사회복지기금)
gotoxy(15,26);
printf("[%d]는 [사회복지 기금 수령처] 입니다.\n", d);
player[a].position = 16; // 위치를 사회복지기금 수령처로
icon_print(a); // 플레이어의 말 출력
gotoxy(15,27);
printf("사회복지기금 수령처에 모금 된 [%d만원] 을 받습니다.\n", welfare);
player[a].money += welfare; // 사회복지기금을 받고
welfare = 0; // 사회복지 기금 탈탈
break;

case 2: // 2. 무인도 (3턴 고립)
gotoxy(15,26);
printf("[%d]는 [무인도] 입니다.\n", d);
player[a].position = 21; // 위치를 무인도로
icon_print(a); // 플레이어의 말 출력
gotoxy(15,27);
printf("무인도에 고립되어, 3턴 동안 플레이를 하지 못하게 됩니다.\n");
player[a].rest = 3;
island(a);
break;

case 3: // 3. 복권 당첨 (플레이어 소지금액 +15)
gotoxy(15,26);
printf("[%d]는 [복권 당첨] 입니다.\n", d);
gotoxy(15,27);
printf("15만원을 받습니다.\n");
player[a].money += 15;
break;

case 4: // 4. 생일축하 (상대플레이어게서 10만원 받기) , 플레이어의 소지금액 +10, 상대 플레이어 소지금액 -10
gotoxy(15,26);
printf("[%d]는 [생일축하] 입니다.\n", d);
gotoxy(15,27);
printf("다른플레이어들에게 10만원 씩 받습니다.\n");
if(a == 0){
player[1].money -= 10;
player[0].money += 10;
} // if

if (a == 1)
player[1].money += 10;
player[0].money -= 10;
break;

case 5: // 5. 건물수리 (아파트 *-10, 빌라*-6, 주택*-3)
gotoxy(15,26);
printf("[%d]는 [건물수리] 입니다.\n", d);
gotoxy(15,27);
printf("소지한 건물들을 수리합니다.\n");

// for문이 돌면서 도시소유주가 누구인걸찾아서
int i; // 도시 수 for 돌릴 거
int total_building_fix; // 수리비 총액
int building_fix; // 건물상태에 따른 수리비 정의
building_fix = 0;
total_building_fix = 0;

for(i=0 ; i <BOARD ; i++) {
if(!strcmp(city[i].owner, player[a].name)) {
if(city[i].development_level == 2) {
building_fix = city[i].development[1]*0.2;
} // if (도시의 발전도가 2이면) {building_fix = 주택가격 *0.1}

else if(city[i].development_level == 3) {
building_fix = city[i].development[1]*0.2 + city[i].development[2]*0.3;
}// else if (도시의 발전도가 3이면) {building_fix = 주택가격*0.1 빌리가격*0.3}

else if(city[i].development_level == 4) {
building_fix = city[i].development[1]*0.2 + city[i].development[2]*0.3 + city[i].development[3]*0.5;
}// else if (도시의 발전도가 4이면) {building_fix = 주택가격*0.1 빌라가격*0.3 아파트가격*0.5}
// 도시의 발전도에 따른 각 도시의 수리비용 정의 끝
} // if 도시의 소유주가 자신인걸 찾아서
total_building_fix += building_fix;
} // for 도시 돌림

gotoxy(15,28);
printf("소유지한 도시의 건물을 수리하시는 가격은 총 [%d만원]입니다.\n", total_building_fix);
player[a].money -= total_building_fix;// 플레이어[a].money -= bulding_fix_sum;
break;

case 6: // 6. 과속운전 (플레이어 소지금액 -5)
gotoxy(15,26);
printf("[%d]는 [과속운전] 입니다.\n", d);
gotoxy(15,27);
printf("벌금으로 5만원을 냅니다.\n");
player[a].money -= 5;
break;
} // switch
}// void gold_key(int a)



void trade(int a) { // 도시구매 // 주택, 빌라, 아파트 건설 // 통행료 납부 거래
BLUE; // 공지 색
// 1. 플레이어의 위치가 시작
if(!strcmp(city[player[a].position].name, "시작")) { // 플레이어가 위치한 칸의 이름이 시작이면
return;
} // if 시작

// 2. 플레이어의 위치가 황금열쇠
else if (!strcmp(city[player[a].position].name, "황금열쇠")) {
sleep(1);
gold_key(a); // 황금열쇠 함수 호출
} // else if 황금열쇠

// 3. 플레이어의 위치가 무인도
else if(!strcmp(city[player[a].position].name, "무인도")) {
player[a].rest = 3;
gotoxy(34,15);
printf("                               ");
gotoxy(34,16);
printf("                               ");
gotoxy(34,17);
printf("                               ");
gotoxy(34,18);
printf("                               ");
gotoxy(34,19);
printf("                               ");
island(a);
} // else if 무인도

// 4. 플레이어의 위치가 사회복지기금 접수처
else if(!strcmp(city[player[a].position].name, "사회복지기금 접수처")) {
gotoxy(15,25);
printf("사회복지기금으로 [20만원]을 기부합니다.\n");
player[a].money -= 20; // 20만원 삥뜯
welfare += 20; // 사회복지기금으로 20만원 들어옴
} // else if 사회복지기금 접수처

// 5. 플레이어의 위치가 사회복지기금 수령처
else if(!strcmp(city[player[a].position].name, "사회복지기금 수령처")) {
// 근데 사회복지기금이 0원이면 억울하니까...
if(welfare == 0) {
gotoxy(15,25);
printf("사회복지기금 [20만원]을 수령합니다.\n");
player[a].money += 20;
} // 사회복지기금 모은게 0원이면

else {
gotoxy(15,25);
printf("사회복지기금 [%d만원]을 수령합니다.\n", welfare);
player[a].money += welfare; // 플레이어가 사회복지기금을 수령받고
welfare = 0; // 사회복지기금 탈탈 
} // 네
} // else if 사회복지기금 접수처


// 6. 플레이어의 위치가 독도일 경우
else if(!strcmp(city[player[a].position].name, "독도")) {
// 1) 독도의 소유자가 없는 상태일 경우
if(!strcmp(city[player[a].position].owner, "없음")) { // 독도의 소유권이 없는 상태라면
gotoxy(15,25);
printf("[독도]가 비어있습니다.\n");
gotoxy(15,26);
printf("[%d만원]을 지불하고 독도를 구매하겠습니까?\n", city[player[a].position].development[0]); // 독도구매 금액

if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++ // 발전도가 1이니까 한개생길듯
player[a].money -= city[player[a].position].development[0]; // 구매
} // if
} // 독도 소유주 없을 때 if끝

// 2) 독도에 도착했을때, 독도가 자신의 소유지라면
else if(!strcmp(city[player[a].position].owner, player[a].name)) { // 독도가 자신의 소유지이면
gotoxy(15,25);
printf("독도는 [%s]님의 섬 입니다.\n", player[a].name);
gotoxy(15,26);
printf("독도는 건물을 세우지 못합니다.\n");
} // else if 독도가 자신의 소유지 일 경우 

// 3) 그외, 독도에 도착했는데 독도가 자신의 소유지가 아니라면 통행료를 내야한다
else {
// 독도가 누구의 소유지임을 알림
if(a == 1) {
gotoxy(15,25);
printf("독도는 [%s]님의 섬입니다.\n", city[player[0].position].owner);
}

else{
gotoxy(15,25);
printf("독도는 [%s]님의 섬입니다.\n", city[player[1].position].owner);
}

gotoxy(15,26);
printf("독도를 지나가기 위해서는 통행료를 납부해야합니다.\n");

int dokdo_pass_price;
dokdo_pass_price = city[player[a].position].development[0]*0.9; // 독도 통행료는 독도인수가격 * 0.9

gotoxy(15,27);
printf("통행료는 [%d만원]을 납부합니다.\n", dokdo_pass_price);

player[a].money -= dokdo_pass_price;

if(a == 0) // 플레이어1인 경우
player[1].money += dokdo_pass_price; // 플레이어 1에게 통행료가 들어와야함 
else
player[0].money += dokdo_pass_price;
} // else 독도가 빈 상태도 아니고 자신의 소유지도 아닐 경우 통행료를 내야함 
// 그 외의 경우 통행료 내야함
} // 독도

// 7. 플레이어가 위치 한 곳이 도시 이름일 경우, 도시의 소유자가 없을 때
else if(!strcmp(city[player[a].position].owner, "없음")) { // 그 외의 경우, 도시 이름이 적힌 말판에 도착, 소유주가 없는 땅 일 경우
gotoxy(15,25);
printf("도시 [%s]가 비어있습니다.\n", city[player[a].position].name);
gotoxy(15,26);
printf("[%d만원]을 지불하고 도시를 구매하겠습니까?\n", city[player[a].position].development[0]); // 0 구매가격

if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[0];
} // if
} // else if 소유주가 없는 땅에 도착

// 8. 플레이어가 위치 한 곳이 도시 이름일 경우, 자신이 소유한 땅에 도착 했을때
else if(!strcmp(city[player[a].position].owner, player[a].name)) { // 자신이 소유한 땅에 도착하면
gotoxy(15,25);
printf("도시 [%s]는 당신의 도시입니다.\n", city[player[a].position].name);

if(city[player[a].position].development_level == 1) { // 발전도가 1, 도시만 구입 한 상태라면
gotoxy(15,26);
printf("[%d만원]을 지불하고, 주택을 세우시겠습니까?\n", city[player[a].position].development[1]);

if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[1];
} // 선택지
} // 발전도가 1이면, 도시만 구입한 상태라면

else if(city[player[a].position].development_level == 2) { // 발전도가 2, 주택을 세운 상태라면
gotoxy(15,26);
printf("[%d만원]을 지불하고, 빌라를 세우시겠습니까?\n", city[player[a].position].development[2]);

if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[2];
} // 선택지
} // 발전도가 2, 주택을 세운 상태라면

else if(city[player[a].position].development_level == 3) { // 발전도가 3, 빌라를 세운 상태라면
gotoxy(15,26);
printf("[%d만원]을 지불하고, 아파트를 세우시겠습니까?\n", city[player[a].position].development[3]);

if(yes_or_no(a)) {
sleep(1);
strcpy(city[player[a].position].owner, player[a].name); // 도시 소유주의 이름에 플레이어 이름을 복사
development_mark(a); // 도시 발전도 마크
city[player[a].position].development_level++; // 발전도++
player[a].money -= city[player[a].position].development[3];
} // 선택지
} // 발전도가 3, 빌라를 세운 상태라면

else if(city[player[a].position].development_level == 4) { // 발전도 만랩, 건물 못세움
gotoxy(15,26);
printf("도시 [%s]는 더이상 건물을 세우지 못합니다.\n", city[player[a].position].name);
} // 발전도가 4, 만랩 건물 못세움
} // else if 자신이 소유한 땅에 도착


// 9. 플레이어가 위치 한 곳이 도시 이름일 경우, 소유주가 남인 땅에 도착 했을때, 통행료를 납부해야한다.
else { // else 소유주가 남인 땅에 도착, 통행료 납부
if(a == 1) {
gotoxy(15,25);
printf("[%s]님의 도시입니다.\n", city[player[1].position].owner);
}

else {
gotoxy(15,25);
printf("[%s]님의 도시입니다\n.", city[player[0].position].owner);
}

gotoxy(15,26);
printf("지나가기 위해서는 통행료를 납부해야합니다.\n");

int pass_price; // 통행료
if(city[player[a].position].development_level == 1) { // 발전도가 1, 도시만 구매한 상태 일 경우
pass_price = city[player[a].position].development[0]*0.5; // 통행료로 도시 가격의 0.5배를 냄
}// 발전도가 1, 도시만 구매한 상태 일 경우

else if(city[player[a].position].development_level == 2) { // 발전도가 2, 주택을 세운 경우
pass_price = city[player[a].position].development[0]*0.5 + city[player[a].position].development[1]*0.2; 
// 통행료로 도시 가격의 0.5배 + 주택가격의 0.2배를 냄
}

else if(city[player[a].position].development_level == 3) { // 발전도가 3, 빌라를 세운 경우
pass_price = city[player[a].position].development[0]*0.5 + city[player[a].position].development[1]*0.2 
+ city[player[a].position].development[2]*0.3; 
// 통행료로 도시 가격의 0.5배 + 주택가격의 0.2배 + 빌라가격의 0.3를 냄
}

else if(city[player[a].position].development_level == 4) { // 발전도가 4, 아파트를 세운 경우
pass_price = city[player[a].position].development[0]*0.5 + city[player[a].position].development[1]*0.2 
+ city[player[a].position].development[2]*0.3 + city[player[a].position].development[3]*0.4; 
// 통행료로 도시 가격의 0.5배 + 주택가격의 0.2배 + 빌라가격의 0.3 + 아파트가격의 0.4배를 냄
} // 발전도가 3, 빌라를 세운 경우

gotoxy(15,27);
printf("통행료 [%d만원]을 납부합니다.\n", pass_price);

player[a].money -= pass_price;
if(a == 0) // 플레이어1인 경우
player[1].money += pass_price; // 플레이어 1에게 통행료가 들어와야함 
else
player[0].money += pass_price;
// printf("[%s]님의 도시입니다.\n", city[player[1].position].owner);

mna(a); // 인수
} // else 소유주가 남인 땅에 도착
} // void trade(int a)



void player_state_update() { // 플레이어의 잔고, 보유 건물 수 업데이트하는 함수
int i;
for (i=0 ; i<PLAYER ; i++) {
if(i==0) { // 플레이어 1이면
YELLOW; // 노란색으로 출력
gotoxy(15,7);
printf("[%s]님 정보\n", player[i].name);
gotoxy(15,8);
printf("소지금 : [%d만원]\n", player[i].money);
}

if(i==1) {
RED;
gotoxy(48,7);
printf("[%s]님 정보\n", player[i].name);
gotoxy(48,8);
printf("소지금 : [%d만원]\n", player[i].money);
}

int j; // 도시 돌림
int city_count1 = 0;
int city_count2 = 0;

for(j=0 ; j<BOARD ; j++) {
if(i == 0) { // 플레이어 1의 소유 도시 수 세기
if(!strcmp(city[j].owner, player[0].name))
city_count1++;
} // if(a==0)

else {
if(!strcmp(city[j].owner, player[1].name))
city_count2++;
}
} // for BOARD의 수 만큼 for문 돌림


if(i==0) { // 플레이어 1이면
YELLOW; // 노란색으로 출력
gotoxy(15,9);
printf("소유 도시 갯수 : [%d개]\n", city_count1);
}

if(i==1) {
RED;
gotoxy(48,9);
printf("소유 도시 갯수 : [%d개]\n", city_count2);
}
} // for 플레이어 수
} // void player_state_update() 



void bankruptcy(int a) { // 파산 검사, 도시 매각 함수
BLUE; // 공지
gotoxy(15,25);
printf("                                                         \n");
gotoxy(15,26);
printf("                                                         \n");
gotoxy(15,27);
printf("                                                         \n");
gotoxy(15,28);
printf("                                                         \n");
int count = 0;

int j; // 플레이어 수 for
for (j=0 ; j<PLAYER ; j++) {
if (player[j].money < 0) {
// 공지위치
gotoxy(15,25);
printf("[%s]님은 파산 위기에 쳐했습니다.\n", player[j].name);
gotoxy(15,26);
printf("소유중인 도시를 처분합니다.\n");

int i; // 도시 수 for
for(i=0 ; i<BOARD ; i++) { // 플레이어의 소유 도시 파악
if(!strcmp(city[i].owner, player[j].name)) {
count++; // 자신이 소유한 도시를 세서 카운트
gotoxy(15,27);
printf("[%s]를 처분하시겠습니까?\n", city[i].name);

if(yes_or_no(a)) {
sleep(1);
int back_price;
if(city[i].development_level == 1) { // 도시의 발전도가 1이라면
back_price = city[i].development[0]*0.9;
} // if 도시의 발전도가 1이라면

else if(city[i].development_level == 2) { // 도시의 발전도가 2이라면, 주택까지
back_price = city[i].development[0]*0.9 + city[i].development[1]*0.9;
}

else if(city[i].development_level == 3) { // 도시의 발전도가 3이라면, 빌라까지
back_price = city[i].development[0]*0.9 + city[i].development[1]*0.9 + city[i].development[2]*0.9;
}

else if(city[i].development_level == 4) { // 도시의 발전도가 4이라면, 아파트까지
back_price = city[i].development[0]*0.9 + city[i].development[1]*0.9 + city[i].development[2]*0.9 + city[i].development[3]*0.9;
}

player[j].money += back_price;
gotoxy(city[i].building_x, city[i].building_y);
printf("          \n");
strcpy(city[i].owner, "없음");
city[i].development_level = 0;
} // 선택지 if
gotoxy(15,27);
printf("                                                         \n");
gotoxy(15,28);
printf("                                                         \n");
} // if 자신이 소유한 도시를 세서 카운트
} // for 플레이어의 소유 도시 파악
} // if
} // for
} // void bankruptcy() 



void mna(int a) { // 도시 인수
int mna_price;

gotoxy(15,25);
printf("                                                         \n");
gotoxy(15,26);
printf("                                                         \n");
gotoxy(15,27);
printf("                                                         \n");
gotoxy(15,28);
printf("                                                         \n");

if(city[player[a].position].development_level == 1) { // 발전도가 1, 도시만 구매한 상태 일 경우
mna_price = city[player[a].position].development[0]*1.1; // 인수가격으로 도시 가격의 1.1배를 냄
}// 인수가격으로 도시가격*1.1

else if(city[player[a].position].development_level == 2) { // 발전도가 2, 주택을 세운 경우
mna_price = city[player[a].position].development[0]*1.1 + city[player[a].position].development[1]*1.1; 
// 인수가격으로 (도시가격 + 주택가격)1*1
}

else if(city[player[a].position].development_level == 3) { // 발전도가 3, 빌라를 세운 경우
mna_price = city[player[a].position].development[0]*1.1 + city[player[a].position].development[1]*1.1 + city[player[a].position].development[2]*1.1; 
// 인수가격으로 (도시 가격+ 주택가격 + 빌라가격)* 1.1
} // 인수가격 정의에 대한 if문 끝

if(player[a].money >= mna_price && city[player[a].position].development_level < 4) { 
// 소지금액이 인수 금액보다 많고 발전도가 4가 아니면

BLUE;
gotoxy(15,25);
printf("[%s]님의 도시 [%s]를 인수 할 수 있습니다.\n",city[player[a].position].owner, city[player[a].position].name);
gotoxy(15,26);
printf("[%d만원]을 주고 [%s]를 인수하시겠습니까?", mna_price, city[player[a].position].name);

if(yes_or_no(a)) {
strcpy(city[player[a].position].owner, player[a].name);
development_mark(a);

if(a == 0) { // 플레이어 1이면
player[1].money += mna_price;
player[0].money -= mna_price;
} // if 플레이어 1

else { // 플레이어2
player[1].money -= mna_price;
player[0].money += mna_price;
} // 플레이어 2이면
} // if yes_or_no(a)
} // if 소지금액이 인수 금액보다 많고 발전도가 4가 아니면
} // void mna(int a)


void island(int a) { // island // 플레이어의 위치가 island일때 호출됨
if (player[a].rest >= 1) {
gotoxy(15,25);
printf("[%s]님은 무인도에 갖혔습니다.\n", player[a].name);
gotoxy(15,26);
printf("주사위를 두 번 던져서 같은 수가 나오면, \n");
gotoxy(15,27);
printf("무인도를 탈출 할 수 있습니다.\n");

sleep(1);
gotoxy(15,25);
printf("                                              \n");
gotoxy(15,26);
printf("                                              \n");
gotoxy(15,27);
printf("                                              \n");
dice_print();


if (dice_num1_info == dice_num2_info) {
player[a].rest = 0;
gotoxy(15,25);
printf("[%s]님이 무인도를 탈출하셨습니다!\n", player[a].name);
gotoxy(15,26);
printf("주사위를 던집니다.");
icon_movement(a);
}// if dice_num1_info == dice_num2_info

else {
gotoxy(15,25);
printf("[%s]님이 무인도 탈출에 실패하셨습니다.\n", player[a].name);
gotoxy(15,26);
player[a].rest--;
printf("[%d턴] 남았습니다.\n", player[a].rest);
} // else
}// if(player[a].rest >= 1)
} // void island(int a)




int yes_or_no() {
char select[10] = "";
gotoxy(15, 28);
printf("[네 /  아니오]로 답변해주시길 바랍니다. ");
scanf("%s", select);

if(!strcmp("네", select)) {
return 1;
}

else 
return 0;

} // int yes_or_no(int a)


/*
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


*/

/*
void opening() {
printf("***************************\n");
printf("시스템 프로그래밍 팀 프로젝트\n");
printf("        주제 : BLUE MARBLE\n");
printf("           20143270 안소정\n");
printf("           20143248 김다슬\n");
printf("***************************\n");
} // void opening()
*/


void ending(int a) {
//system("cls");
BLUE;
gotoxy(15,25);
printf("[%s]님이 승리하셨습니다!\n", player[a].name);
gotoxy(15,26);
printf("재산 : [%d만원]\n", player[a].money);

int i; // 도시 세는 함수
int city_count;
for(i=0 ; i<BOARD ; i++) {
if(a == 0) { // 플레이어 1가 승리
if(!strcmp(city[i].owner, player[0].name))
city_count++;
} // if(a==0)

else {
if(!strcmp(city[i].owner, player[1].name))
city_count++;
} // else 플레이어 2가 승리
}
gotoxy(15,27);
printf("소유한 도시 : %d개\n", city_count);
//printf("** 소유 도시 목록 **\n");
// 여건되면 도시 뭐를 소유했는지도 올리기
/*
int j;// if a가 뭐무녀을 넣어야하나....
for(j=0 ; i<BOARD ; i++) {
if(!strcmp(city[i].owner, player[a].name))
city_count++;
}*/
}


void main() { // 메인
BLUE;
printmap();
printna();


int i = 0;
insert_playername(i);
clear();
player_state();

while(1) {
player_state_update();

BLUE;
icon_movement(i);
trade(i);
BLUE;
bankruptcy(i);
// print_information(i);
sleep(3);
clear();

if(i==1) // 순서 바꾸기
i=0;
else
i++;
} // while(1) -> 무한루프

}


