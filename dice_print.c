#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // time
#include <unistd.h> // Sleep, 사용자 함수 kbhit 구현
#include <termio.h> // 사용자 함수 getch 구현
#include <termios.h> // 사용자 함수 kbhit 구현
#include <fcntl.h> // 사용자 함수 kbhit 구현

int dice_cast() { // 주사위 숫자 생성해주는 함수
int dice_num;

srand(time(NULL)); // 시간에 따라 변화하는 랜덤 함수
dice_num = rand()%6 + 1; // 1~6 사이 난수 생성

return dice_num;
} // int dice_case()

void gotoxy(int x, int y) { // 좌표 설정 함수
printf("\033[%d;%df",y,x);
fflush(stdout);
} // void gotoxy(int x, int y)

void dice_print(){ // 주사위 눈의 수에 따른 주사위 모양 출력
int dice_num1 = 5;
int dice_num2 = 0;

dice_num1 = dice_cast(); // 주사위 첫번째 숫자
dice_num2 = dice_cast(); // 주사위 첫번째 숫자

if(dice_num1 == 1) {
gotoxy(33,8);
printf("┌─────────┐");
gotoxy(33,9);
printf("│	  │\n");
gotoxy(33,10);
printf("│   ●     │\n");
gotoxy(33,11);
printf("│         │\n");
gotoxy(33,12);
printf("└─────────┘\n");
} // if(dice_num1 == 1)

else if(dice_num1 == 2) {
gotoxy(33,8);
printf("┌─────────┐");
gotoxy(33,9);
printf("│ ●	  │\n");
gotoxy(33,10);
printf("│	  │\n");
gotoxy(33,11);
printf("│     ●   │\n");
gotoxy(33,12);
printf("└─────────┘\n");
} // else if(dice_num1 == 2)

else if(dice_num1 == 3) {
gotoxy(33,8);
printf("┌─────────┐");
gotoxy(33,9);
printf("│ ●	  │\n");
gotoxy(33,10);
printf("│   ●     │\n");
gotoxy(33,11);
printf("│     ●   │\n");
gotoxy(33,12);
printf("└─────────┘\n");
} // else if(dice_num1 == 3)

else if(dice_num1 == 4) {
gotoxy(33,8);
printf("┌─────────┐");
gotoxy(33,9);
printf("│ ●    ●  │\n");
gotoxy(33,10);
printf("│	  │\n");
gotoxy(33,11);
printf("│ ●    ●  │\n");
gotoxy(33,12);
printf("└─────────┘\n");
} // else if(dice_num1 == 4)

else if(dice_num1 == 5) {
gotoxy(33,8);
printf("┌─────────┐");
gotoxy(33,9);
printf("│ ●    ●  │\n");
gotoxy(33,10);
printf("│    ●    │\n");
gotoxy(33,11);
printf("│ ●    ●  │\n");
gotoxy(33,12);
printf("└─────────┘\n");
} // else if(dice_num1 == 5)

else if(dice_num1 == 6) {
gotoxy(33,8);
printf("┌─────────┐");
gotoxy(33,9);
printf("│ ●    ●  │\n");
gotoxy(33,10);
printf("│ ●    ●  │\n");
gotoxy(33,11);
printf("│ ●    ●  │\n");
gotoxy(33,12);
printf("└─────────┘\n");
} // else if(dice_num1 == 6)



if(dice_num2 == 1) {
gotoxy(44,8);
printf("┌─────────┐");
gotoxy(44,9);
printf("│	  │\n");
gotoxy(44,10);
printf("│   ●     │\n");
gotoxy(44,11);
printf("│         │\n");
gotoxy(44,12);
printf("└─────────┘\n");
} // if(dice_num2 == 1)

else if(dice_num2 == 2) {
gotoxy(44,8);
printf("┌─────────┐");
gotoxy(44,9);
printf("│ ●	  │\n");
gotoxy(44,10);
printf("│	  │\n");
gotoxy(44,11);
printf("│     ●   │\n");
gotoxy(44,12);
printf("└─────────┘\n");
} // else if(dice_num2 == 2)

else if(dice_num2 == 3) {
gotoxy(44,8);
printf("┌─────────┐");
gotoxy(44,9);
printf("│ ●	  │\n");
gotoxy(44,10);
printf("│   ●     │\n");
gotoxy(44,11);
printf("│     ●   │\n");
gotoxy(44,12);
printf("└─────────┘\n");
} // else if(dice_num2 == 3)

else if(dice_num2 == 4) {
gotoxy(44,8);
printf("┌─────────┐");
gotoxy(44,9);
printf("│ ●    ●  │\n");
gotoxy(44,10);
printf("│	  │\n");
gotoxy(44,11);
printf("│ ●    ●  │\n");
gotoxy(44,12);
printf("└─────────┘\n");
} // else if(dice_num2 == 4)

else if(dice_num2 == 5) {
gotoxy(44,8);
printf("┌─────────┐");
gotoxy(44,9);
printf("│ ●    ●  │\n");
gotoxy(44,10);
printf("│    ●    │\n");
gotoxy(44,11);
printf("│ ●    ●  │\n");
gotoxy(44,12);
printf("└─────────┘\n");
} // else if(dice_num2 == 5)

else if(dice_num2 == 6) {
gotoxy(44,8);
printf("┌─────────┐");
gotoxy(44,9);
printf("│ ●    ●  │\n");
gotoxy(44,10);
printf("│ ●    ●  │\n");
gotoxy(44,11);
printf("│ ●    ●  │\n");
gotoxy(44,12);
printf("└─────────┘\n");
} // else if(dice_num1 == 6)
//gotoxy(44,8);
//printf("   주사위1 : %d", dice_num1 );

}

void main() {
dice_print();
}
