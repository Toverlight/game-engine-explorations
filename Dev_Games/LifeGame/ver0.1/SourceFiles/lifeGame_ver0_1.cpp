/*LifeGame by Toverlight*/
/*cpp started on 1/10/2024*/
/*version 0.1 1/10/2024*/
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#define WIDTH (100)//WIDTH corresponds to x
#define HEIGHT (40)//HEIGHT corresponds to y
#define DELAY_MS (16 * 2)//延迟的时间设置

/*游戏规则：
* 1.元胞周围有2个生元胞时，该元胞状态不变
* 2.元胞周围有3个生元胞时，该元胞为生
* 3.其余情况下，该元胞为死
*/

typedef struct {
	char status;//status为1进行演绎，为0暂停演绎
}Info;
typedef struct {
	char status;//元胞的状态：1为生，0为死
	char ch;//表示元胞的字符
}Cell;

//将光标移至x,y处
//x水平轴，y竖直轴
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

//初始化游戏
void startup(Info* info,Cell cells[])
{
	srand((unsigned)time(NULL));
	//初始状态设置
	info->status = 1;
	for (int j = 0; j < HEIGHT + 2; j++)
	{
		for (int i = 0; i < WIDTH + 2; i++)
		{
			if (i == 0 || i == WIDTH + 1 || j == 0 || j == HEIGHT + 1)//外围一圈是“边界”，设为0方便处理
			{ 
				cells[j * (WIDTH + 2) + i].status = 0;
			}
			else//里面的1~HEIGHT(WIDTH)才是展示的元胞
			{
				cells[j * (WIDTH + 2) + i].status = rand() % 2;//0或1
				cells[j * (WIDTH + 2) + i].ch = '#';
			}
		}
	}

}
//显示
void show(Info* info,Cell cells[])
{
	if (info->status == 1)
	{
		gotoxy(0, 0);
		//显示元胞
		for (int j = 1; j < HEIGHT + 1; j++)
		{
			for (int i = 1; i < WIDTH + 1; i++)
			{
				if (cells[j * (WIDTH + 2) + i].status)//活着
				{
					printf("%c", cells[j * (WIDTH + 2) + i].ch);
				}
				else//死了
				{
					printf(" ");
				}
			}
			printf("\n");
		}
	}
}
//与用户输入无关的更新
void updateWithoutInput(Info* info,Cell cells[],char statusCopy[])
{
	static int environmentSurviverSum = 0;
	if (info->status == 1)
	{
		//对每一个元胞当前环境进行判断，修改元胞状态
		for (int j = 1; j < HEIGHT + 1; j++)
		{
			for (int i = 1; i < WIDTH + 1; i++)
			{
				//对周围的八个元胞存活情况进行统计
				environmentSurviverSum = cells[(j - 1) * (WIDTH + 2) + (i - 1)].status
					+ cells[(j - 1) * (WIDTH + 2) + i].status
					+ cells[(j - 1) * (WIDTH + 2) + (i + 1)].status
					+ cells[j * (WIDTH + 2) + (i - 1)].status
					+ cells[j * (WIDTH + 2) + (i + 1)].status
					+ cells[(j + 1) * (WIDTH + 2) + (i - 1)].status
					+ cells[(j + 1) * (WIDTH + 2) + i].status
					+ cells[(j + 1) * (WIDTH + 2) + (i + 1)].status;
				//根据情况将要修改成的状态值暂存至副本
				if ((environmentSurviverSum) == 3)//生的条件
				{
					statusCopy[j * (WIDTH + 2) + i] = 1;
				}
				else if (environmentSurviverSum != 2)//死的条件
				{
					statusCopy[j * (WIDTH + 2) + i] = 0;
				}
				else//不变的条件
				{
					statusCopy[j * (WIDTH + 2) + i] = cells[j * (WIDTH + 2) + i].status;
				}
			}
		}
		//更新元胞状态
		for (int j = 1; j < HEIGHT + 1; j++)
		{
			for (int i = 1; i < WIDTH + 1; i++)
			{
				cells[j * (WIDTH + 2) + i].status = statusCopy[j * (WIDTH + 2) + i];
			}
		}
	}
}
//与用户输入有关的更新
void updateWithInput(Info* info,char* re)
{
	static char ch;
	//当检测到输入
	if (_kbhit())
	{
		ch = _getch();
		if (ch == ' ')//暂停与继续游戏
		{
			if (info->status) info->status = 0;//暂停
			else info->status = 1;//继续
		}
		else if (ch == 'q')//退出
		{
			exit(0);
		}
		else if (ch == 'r')//重新开始
		{
			*re = 1;
			return;
		}
	}
}
//延迟
void Delay()
{
	Sleep(DELAY_MS);
}

int main()
{
	NewGame:
	char restartFlag = 0;
	char* re = &restartFlag;
	HideCursor();
	Info info;
	Cell cells[(WIDTH + 2) * (HEIGHT + 2)];
	char statusCopy[(WIDTH + 2) * (HEIGHT + 2)];
	startup(&info,cells);
	while (1)
	{
		show(&info,cells);
		updateWithoutInput(&info,cells,statusCopy);
		updateWithInput(&info,re);
		if (restartFlag == 1)
		{
			goto NewGame;
		}
		Delay();
	}

	return 0;
}