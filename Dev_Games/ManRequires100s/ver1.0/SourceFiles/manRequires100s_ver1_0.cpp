/*ManRequires100s by Toverlight*/
/*cpp started on 1/10/2024*/
/*version 1.0 1/11/2024*/
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#define WIDTH (100)//WIDTH corresponds to x
#define HEIGHT (40)//HEIGHT corresponds to y
#define DELAY_MS (16)//延迟的时间设置
#define ENEMY_NUM (3)
#define ENEMY_REFRESH_INTERVAL (5)//敌人刷新位置的帧数间隔，标示敌人反应时间平均数值

//游戏信息
typedef struct {
	char failFlag;//游戏结束标识符
	unsigned long ms;//坚持的秒数
	int difficulty;//游戏难度
}Info;
//玩家
typedef struct {
	char ch;//表示玩家的字符
	int x;//玩家的坐标
	int y;//玩家的坐标
	int last_x;//玩家的上一个坐标
	int last_y;//玩家的上一个坐标
}Player;
//敌人
typedef struct {
	char ch;//表示敌人的字符
	int x;//敌人的坐标
	int y;//敌人的坐标
	int last_x;//敌人的上一个坐标
	int last_y;//敌人的上一个坐标
	int reflectTime;//反应时间，直接关系到敌人的敏捷度
	int prepareDegree;//计数器，增加到反应时间敌人行动一帧并清零
}Enemy;

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
void startup(Info* info,Player* player,Enemy enemies[])
{
	srand((unsigned)time(NULL));
	//游戏信息初始化
	info->ms = 0;
	info->failFlag = 0;
	//玩家初始化
	player->ch = '@';
	player->last_x = -1;
	player->last_y = -1;
	int corner = rand() % 4;//左上、右上、左下、右下四个块中的随机一个块选为玩家出生点
	if (corner == 0)//左上
	{
		player->x = rand() % ((WIDTH - 2) / 4) + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + 1;
	}
	else if (corner == 1)//右上
	{
		player->x = rand() % ((WIDTH - 2) / 4) + (WIDTH - 2) / 3 + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + 1;
	}
	else if (corner == 2)//左下
	{
		player->x = rand() % ((WIDTH - 2) / 4) + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + (HEIGHT - 2) / 3 + 1;
	}
	else if (corner == 3)//右下
	{
		player->x = rand() % ((WIDTH - 2) / 4) + (WIDTH - 2) / 3 + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + (HEIGHT - 2) / 3 + 1;
	}
	//敌群初始化
	for (int k = 0; k < ENEMY_NUM * info->difficulty; k++)//在中间一块区域内生成
	{
		enemies[k].ch = '#';
		//先全设为-1
		enemies[k].x = -1;
		enemies[k].y = -1;
		enemies[k].last_x = -1;
		enemies[k].last_y = -1;
	}
	//不重复位置地生成敌群
	int temp_x, temp_y,reFlag = 1;
	for (int k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		while (reFlag)//重抽
		{
			temp_x = rand() % ((WIDTH - 2) / 5) + WIDTH / 2 + 1;
			temp_y = rand() % ((HEIGHT - 2) / 5) + HEIGHT / 2 + 1;
			reFlag = 0;
			for (int i = 0; i < ENEMY_NUM * info->difficulty; i++)
			{
				if (k == i)continue;
				if (temp_x == enemies[i].x && temp_y == enemies[i].y)
				{
					reFlag = 1;
					break;
				}
			}
		}
		enemies[k].x = temp_x;
		enemies[k].y = temp_y;
		//为每个敌人随机生成其反应能力数值，增加敌人多样性
		enemies[k].reflectTime = rand() % 5 - 2 + ENEMY_REFRESH_INTERVAL;//[-10,10] ∩ Z + ave
		//计数器清零
		enemies[k].prepareDegree = 0;
	}
}
//清屏
void clearScreen()
{
	gotoxy(0, 0);
	for (int j = 0; j < HEIGHT + 1; j++)
	{
		for (int i = 0; i < WIDTH + 1; i++)
		{
			printf(" ");
		}
		printf("\n");
	}
}
//主界面
void mainMenu(Info* info)
{
	//显示游戏标题 
	gotoxy(WIDTH / 2 - 4, 4);
	printf("是男人就坚持100秒");
	//显示开始游戏的条件
	gotoxy(WIDTH / 2 - 7, 7);
	printf(">>>按空格键开始游戏<<<");
	//显示其他详情
	gotoxy(0, 10);
	printf("你将扮演一名玩家，躲避多个敌人的追击\n");
	printf("坚持一百秒，你就赢了\n");
	printf("而中途碰到敌人，游戏结束并显示你坚持的秒数\n");
	printf("加油，真正的man!\n");	
	//检测玩家是否按下开始按键
	char ch;
	while (1)
	{
		ch = _getch();
		if (ch == ' ')
		{
			break;
		}
		Sleep(10);
	}
	//清屏
	clearScreen();
	//玩家输入要挑战的游戏难度
	gotoxy(HEIGHT / 2 - 4, 7);
	printf("请输入你要挑战的游戏难度\n");
	printf("难度范围从1到5，数值越大敌人越多\n");
	//难度获取
	char diff;
	while (1)
	{
		diff = _getch();
		if (diff >= '1' && diff <= '5')
		{
			info->difficulty = diff - '0';
			//清屏
			clearScreen();
			return;
		}
		Sleep(10);
	}
}
//显示
void show(Info* info,Player* player,Enemy enemies[])
{
	gotoxy(0, 0);
	//显示边界、玩家和敌群
	for (int j = 0; j < HEIGHT + 1; j++)
	{
		for (int i = 0; i < WIDTH; i++)
		{
			if (j == 0 || j == HEIGHT - 1)//横向边框
			{
				gotoxy(i, j);
				printf("-");
			}
			else if (i == 0 || i == WIDTH - 1)//纵向边框
			{
				gotoxy(i, j);
				printf("|");
			}
		}
	}
	//显示玩家
	if (player->last_x != player->x || player->last_y != player->y)
	{
		gotoxy(player->last_x, player->last_y);
		printf(" ");

		gotoxy(player->x, player->y);
		printf("%c", player->ch);
	}
	//显示每个敌人
	for (int k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		if (enemies[k].last_x != enemies[k].x || enemies[k].last_y != enemies[k].y)
		{
			gotoxy(enemies[k].last_x, enemies[k].last_y);
			printf(" ");

			gotoxy(enemies[k].x, enemies[k].y);
			printf("%c", enemies[k].ch);
		}
	}
	//显示坚持的秒数
	gotoxy(0, HEIGHT);
	printf("坚持的秒数：%lu.%lu         \n", info->ms / 1000,info->ms % 1000);
}
//与用户输入无关的更新
void updateWithoutInput(Info* info,Player* player,Enemy enemies[])
{
	static int temp_x, temp_y, reFlag = 1;
	//每个敌人的位置更新
	for (int k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		if (enemies[k].prepareDegree < enemies[k].reflectTime)
		{
			enemies[k].prepareDegree++;
		}
		else if (enemies[k].prepareDegree == enemies[k].reflectTime)
		{
			//重置reFlag
			reFlag = 1;
			//敌人尝试靠近玩家
			//还要保证刷新后的位置不与其他敌人重复
			if (reFlag == 1)
			{
				//迫近行动
				//x坐标第一次生成
				if (player->x > enemies[k].x)
				{
					temp_x = enemies[k].x + 1;
				}
				else if (player->x < enemies[k].x)
				{
					temp_x = enemies[k].x - 1;
				}
				else
				{
					temp_x = enemies[k].x;
				}
				//y坐标第一次生成
				if (player->y > enemies[k].y)
				{
					temp_y = enemies[k].y + 1;
				}
				else if (player->y < enemies[k].y)
				{
					temp_y = enemies[k].y - 1;
				}
				else
				{
					temp_y = enemies[k].y;
				}
			}
			//判断是否有坐标重复
			for (int i = 0; i < ENEMY_NUM * info->difficulty; i++)
			{
				if (k == i)continue;//自己不检测
				if (temp_x == enemies[i].x && temp_y == enemies[i].y)
				{
					reFlag++;
					break;
				}
			}
			//如果不重复
			if (reFlag == 1)
			{
				enemies[k].last_x = enemies[k].x;
				enemies[k].last_y = enemies[k].y;
				enemies[k].x = temp_x;
				enemies[k].y = temp_y;
				enemies[k].prepareDegree = 0;
				continue;
			}
			//第一次生成出现重复
			if (reFlag == 2)
			{
				//随机行动（确保不撞墙）
				do
				{
					//x坐标第二次生成
					temp_x = rand() % 2 * 2 - 1 + enemies[k].x;
					//y坐标第二次生成
					temp_y = rand() % 2 * 2 - 1 + enemies[k].y;
				} while (temp_x < 1 || temp_x > WIDTH - 2 || temp_y < 1 || temp_y > HEIGHT - 2);
			}
			//判断是否有坐标重复
			for (int i = 0; i < ENEMY_NUM * info->difficulty; i++)
			{
				if (k == i)continue;//自己不检测
				if (temp_x == enemies[i].x && temp_y == enemies[i].y)
				{
					reFlag++;
					break;
				}
			}
			//如果不重复
			if (reFlag == 2)
			{
				enemies[k].last_x = enemies[k].x;
				enemies[k].last_y = enemies[k].y;
				enemies[k].x = temp_x;
				enemies[k].y = temp_y;
				enemies[k].prepareDegree = 0;
				continue;
			}
			//第二次生成出现重复
			if (reFlag == 3)
			{
				//本次不行动
				//第三次生成
				enemies[k].last_x = enemies[k].x;
				enemies[k].last_y = enemies[k].y;
				enemies[k].prepareDegree = 0;
				continue;
			}
		}
	}
	//判断玩家与敌人是否重合（坐标重合游戏结束）
	for (int k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		if (player->x == enemies[k].x && player->y == enemies[k].y)
		{
			info->failFlag = 1;
			break;
		}
	}
}
//与用户输入有关的更新
void updateWithInput(Player* player)
{
	static char ch;
	if (_kbhit())//当检测到输入
	{
		ch = _getch();
		if (ch == 'w' && player->y > 1)//向上移动
		{
			player->last_x = player->x;
			player->last_y = player->y;
			player->y--;
		}
		else if (ch == 'a' && player->x > 1)//向左移动
		{
			player->last_x = player->x;
			player->last_y = player->y;
			player->x--;
		}
		else if (ch == 's' && player->y < HEIGHT - 2)//向下移动
		{
			player->last_x = player->x;
			player->last_y = player->y;
			player->y++;
		}
		else if (ch == 'd' && player->x < WIDTH - 2)//向右移动
		{
			player->last_x = player->x;
			player->last_y = player->y;
			player->x++;
		}
	}
}
//游戏结束的处理（赢或输）
void Process(Info* info,Enemy enemies[])
{
	//敌群动态数组内存的回收
	free(enemies);

	//显示与玩家输入
	if (info->failFlag == 0)
	{
		gotoxy(20,HEIGHT);
		printf("你赢了，真正的man!\n\n");
	}
	else if (info->failFlag == 1)
	{
		gotoxy(20, HEIGHT);
		printf("你被敌人抓住了！\n\n");
	}
	printf("摁下空格键退出游戏<<<\n");
	char ch;
	while (1)
	{
		if (_kbhit())
		{
			ch = _getch();
			if (ch == ' ')
			{
				return;
			}
		}
		Sleep(10);
	}
}
//延迟
void Delay()
{
	Sleep(DELAY_MS);
}

int main()
{
	HideCursor();
	Info info;
	Player player;
	mainMenu(&info);
	Enemy* enemies = (Enemy*)malloc(ENEMY_NUM * info.difficulty * sizeof(Enemy));
	startup(&info,&player,enemies);
	clock_t startTime = clock();//程序开始时间
	while (info.failFlag == 0 && info.ms < 100*1000)
	{
		clock_t currentTime = clock();//程序当前时间
		info.ms = (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;  // 计算经过的毫秒数

		show(&info,&player,enemies);
		updateWithoutInput(&info,&player,enemies);
		updateWithInput(&player);
		Delay();
	}
	Process(&info,enemies);
	return 0;
}