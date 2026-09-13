/*PlaneBattle by Toverlight*/
/*cpp started on 1/8/2024*/
/*version 0.1 1/8/2024*/
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

//全局变量
int high = 30, width = 25;//场景大小
int position_x, position_y;//玩家飞机的坐标
int bullet_x, bullet_y;//子弹的坐标
int enemy_x, enemy_y;//敌机的坐标
int* enemy_px; int* enemy_py;//指向敌机坐标，方便修改，刷新函数用到
int bullet_speed, enemy_speed;//子弹与敌机的速度
int score;

//将光标移至x,y处
void gotoxy(int x, int y) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(handle, pos);
}
//用于隐藏光标
void HideCursor() {
	CONSOLE_CURSOR_INFO cursor_info = { 1,0 };//0表示隐藏光标
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}

//初始化游戏
void startup() {
	srand((unsigned)time(NULL));

	position_x = high / 2;
	position_y = width / 2;
	enemy_y = rand() % width;
	enemy_py = &enemy_y;
	enemy_x = 0;
	enemy_px = &enemy_x;
	bullet_x = -1;
	bullet_y = -1;

	bullet_speed = 1;//数字越大速度越小（误导）
	enemy_speed = 5;

	score = 0;
}

//显示
void show() {
	gotoxy(0, 0);//将光标移至原点处

	for (int i = 0; i < high; i++) {
		for (int j = 0; j < width; j++) {
			if ((i == position_x) && (j == position_y))
			{
				printf("*");//打印玩家飞机
				continue;
			}
			if ((i == bullet_x)&&(j == bullet_y))
			{
				printf("|");//打印子弹
				continue;
			}
			if ((i == enemy_x) && (j == enemy_y))
			{
				printf("@");//打印敌机
				continue;
			}
			printf(" ");
		}
		printf("\n");
	}
	printf("得分：%d   \n", score);



}
//刷新敌机
void refreshEnemy(int* enemy_x, int* enemy_y){
	*enemy_x = 0;
	*enemy_y = rand() % width;
}

//与输入无关的处理
void updateWithoutInput() {
	static int b_speed = 0,e_speed = 0;
	//子弹飞行
	if (b_speed < bullet_speed)
	{
		b_speed++;
	}
	else
	{
		b_speed = 0;
		if (bullet_x >= 0)
		{
			bullet_x--;
		}
		else
		{
			bullet_x = -1;
			bullet_y = -1;
		}
	}

	//判断敌机与子弹是否碰撞
	if ((bullet_x == enemy_x) && (bullet_y == enemy_y))
	{
		refreshEnemy(enemy_px, enemy_py);
		bullet_x = -1;
		score++;
	}

	//判断敌机是否越过底部
	if (enemy_x == high)
	{
		score--;
		refreshEnemy(enemy_px, enemy_py);
	}

	//敌机下行
	if (e_speed < enemy_speed)
	{
		e_speed++;
	}
	else
	{
		e_speed = 0;
		if (enemy_x < high)
		{
			enemy_x++;
		}
		else if (enemy_x >= high)
		{
			refreshEnemy(enemy_px, enemy_py);
		}
	}
	
}

//与输入有关的处理
void updateWithInput() {
	char input;

	if (_kbhit())//当按按键时执行
	{
		input = _getch();//从控制台获取一个输入，但不显示出来
		if (input == ' ') //发射子弹
		{
			bullet_x = position_x - 1;
			bullet_y = position_y;
		}
		//控制飞机移动
		if (input == 'w')
		{
			if (position_x > 0)
				position_x--;
		}
		if (input == 'a')
		{
			if (position_y > 0)
				position_y--;
		}
		if (input == 's')
		{
			if (position_x < high - 1)
				position_x++;
		}
		if (input == 'd')
		{
			if (position_y < width - 1)
				position_y++;
		}
		

	}
}

int main() {
	HideCursor();
	startup();
	while (1) {
		show();
		updateWithoutInput();
		updateWithInput();
		Sleep(16);
	}
	return 0;
}