/*BaunceBall by Toverlight*/
/*cpp started on 1/9/2024*/
/*version 0.1 1/10/2024*/
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#define HEIGHT (40)
#define WIDTH (60)

typedef struct {
	int x;
	int y;
	int last_x;
	int last_y;
	int x_speed;
	int y_speed;
	int velocity;
	char ch;
}Ball;
typedef struct {
	int x;
	int y;
	int last_x;
	int last_y;
	int radius;
	char ch;
}Board;
typedef struct {
	int score;
	int failFlag;
}Info;

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
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
//游戏初始化
void startup(Ball* ball,Board* board,Info* info)
{
	info->failFlag = 0;//failFlag为1就是gameover
	info->score = 0;
	srand((unsigned)time(NULL));
	//
	//球的位置
	ball->x = 1;
	ball->y = rand() % (WIDTH - 4);
	ball->ch = '@';

	//球的速度
	ball->x_speed = rand() % 2 * 2 - 1;//-1或1
	ball->y_speed = rand() % 2 * 2 - 1;//-1或1
	
	//球每刷新一帧的实际经过帧
	ball->velocity = 2;

	//板的位置
	board->x = HEIGHT - 2;
	board->y = WIDTH / 2;
	board->ch = '#';

	//板的半径
	board->radius = 5;//分别向左右延伸radius格

}
//显示
void show(Ball* ball,Board* board,Info* info)
{
	gotoxy(0, 0);
	//绘制球
	if (ball->x != ball->last_x || ball->y != ball->last_y)
	{
		gotoxy(ball->last_y, ball->last_x);
		printf(" ");	

		gotoxy(ball->y, ball->x);
		printf("%c", ball->ch);
	}
	//绘制板
	if (board->y != board->last_y)
	{
		gotoxy(board->last_y - board->radius, board->x);
		for (int i = board->last_y - board->radius; i <= board->last_y + board->radius; i++)
		{
			printf(" ");
		}

		gotoxy(board->y - board->radius, board->x);
		for (int i = board->y - board->radius; i <= board->y + board->radius; i++)
		{
			printf("%c", board->ch);
		}
	}
	//显示边框
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (i == 0 || i == HEIGHT - 1)
			{
				gotoxy(j, i);
				printf("-");
			}
			else if (j == 0 || j == WIDTH - 1)
			{
				gotoxy(j, i);
				printf("|");
			}
		}
	}
	//显示得分
	gotoxy(0, HEIGHT);
	printf("得分：%d", info->score);
}

//与输入无关的更新
void updateWithoutInput(Ball* ball,Board* board,Info* info)
{
	static int v = 0;
	ball->last_x = ball->x;
	ball->last_y = ball->y;

	//球的飞行与碰撞处理
	if (v < ball->velocity)
	{
		v++;
	}
	else
	{
		if (ball->x == 1)//撞上墙
		{
			ball->x_speed = 1;
		}
		if (ball->y == 1 || ball->y == WIDTH - 2)//撞左墙或右墙
		{
			ball->y_speed = -ball->y_speed;
		}
		if (ball->x == board->x - 1 && ball->y >= board->y - board->radius
			&& ball->y <= board->y + board->radius)//撞到板了
		{
			ball->x_speed = -1;
			info->score++;
		}
		if (ball->x == HEIGHT - 1)//撞下墙
		{
			info->failFlag = 1;
		}
		//游戏没结束，就更新球的位置
		if (!info->failFlag)
		{
			ball->x += ball->x_speed;
			ball->y += ball->y_speed;
		}
		v = 0;
	}
}
//与输入有关的更新
void updateWithInput(Board* board)
{
	board->last_x = board->x;
	board->last_y = board->y;
	static char ch;
	//检测到有键盘输入
	if (_kbhit())
	{
		ch = _getch();
		if (ch == 'a' && board->y - board->radius > 1)//左移
		{
			board->y--;
		}
		if (ch == 'd' && board->y + board->radius < WIDTH - 2)//右移
		{
			board->y++;
		}
	}
}
//失败处理
void failProcess(Info* info)
{
	//将光标移至得分提示栏
	gotoxy(0, HEIGHT);
	//先清空该栏
	for (int i = 0; i < WIDTH; i++)
	{
		printf(" ");
	}
	//再打印gameover和score
	gotoxy(WIDTH / 2 - 4, HEIGHT);
	printf("GameOver!");
	gotoxy(WIDTH / 2 - 5, HEIGHT + 1);
	printf("您的得分是：%d\n", info->score);
}

//延迟
void Delay()
{
	Sleep(4);
}

int main()
{
	HideCursor();
	Ball ball;
	Board board;
	Info info;
	startup(&ball,&board,&info);
	while (info.failFlag == 0) //没输，游戏就继续进行
	{
		show(&ball, &board,&info);
		updateWithoutInput(&ball,&board,&info);
		updateWithInput(&board);
		Delay();
	}
	if (info.failFlag)
	{
		failProcess(&info);
	}
	if (getchar() == '\n')
	{
		exit(0);
	};
	return 0;
}