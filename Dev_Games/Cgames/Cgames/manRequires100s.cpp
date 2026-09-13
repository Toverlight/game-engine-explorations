/*ManRequires100s by Toverlight*/
/*cpp started on 1/10/2024*/
/*version 1.2.1 1/13/2024*/
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <math.h>

#define WIDTH (130)//WIDTH corresponds to x
#define HEIGHT (42)//HEIGHT corresponds to y
#define DELAY_MS (12)//延迟的时间设置
#define ENEMY_NUM (8)//敌人基本数量，随难度调整
#define ENEMY_REFRESH_INTERVAL (6)//敌人刷新位置的帧数间隔，标示敌人反应时间平均数值
#define DIAG_SPEEDUP_RATE (60 * 100)//敌人对角加速概率
#define OBSTACLES_NUM (999)//障碍物最大数量，多设置一些防止下标越界
#define MAX_BULLET_NUM (999)//场上同时容许存在的子弹最大数量
#define BULLET_REFRESH_INTERVAL (3)//子弹刷新位置的帧数间隔

//游戏信息
typedef struct {
	unsigned long ms;//坚持的秒数
	int difficulty;//游戏难度
	char failFlag;//游戏结束标识符
	int bulletsQueueHeadPtr;//子弹循环队列头指针
	int bulletsQueueTailPtr;//子弹循环队列尾指针
}Info;
//玩家
typedef struct {
	int x;//玩家的坐标
	int y;//玩家的坐标
	int last_x;//玩家的上一个坐标
	int last_y;//玩家的上一个坐标
	char ch;//表示玩家的字符
	char direction;//玩家的朝向
}Player;
//敌人
typedef struct {
	int x;//敌人的坐标
	int y;//敌人的坐标
	int last_x;//敌人的上一个坐标
	int last_y;//敌人的上一个坐标
	int reflectTime;//反应时间，直接关系到敌人的敏捷度
	int prepareDegree;//计数器，增加到反应时间敌人行动一帧并清零
	char ch;//表示敌人的字符
	char hitWallStep;//敌人撞墙后，朝一个方向走多少步
	char xyFlag;//朝一个方向走（0上1右2下3左）
	char trappedTime;//敌人被圆形子弹射中后会无法行动trappedTime秒
	clock_t startTime;//敌人的实时时钟，被子弹射中的同时记录下当前时间
}Enemy;
//J键子弹
typedef struct {
	int x;//子弹的坐标
	int y;//子弹的坐标
	int last_x;//子弹的上一个坐标
	int last_y;//子弹的上一个坐标
	char ch;//显示子弹的字符，建议用对称字符
	char direction;//子弹的发射方向
	char isDestroyed;//子弹是否被摧毁（遇敌，碰障碍物等）
}Bullet;
//障碍物
typedef struct {
	int x;
	int y;
	char ch;//表示障碍物的字符
	char isDestroyed;//障碍物是否被摧毁
	char removeFlag;//障碍物摧毁后需要用空格抹除
}Obstacle;
int obstacleNum = 0;//障碍物的实际数量
int trappedTime = 0;//敌人受困时间
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
void startup(Info* info,Player* player,Enemy enemies[],Obstacle obstacles[],Bullet bullets[])
{
	srand((unsigned)time(NULL));
	//游戏信息初始化
	info->ms = 0;
	info->failFlag = 0;
	//玩家初始化
	player->ch = '@';
	player->last_x = -1;
	player->last_y = -1;
	player->direction = 1;
	int corner = rand() % 4;//左上、右上、左下、右下四个块中的随机一个块选为玩家出生点
	if (corner == 0)//左上
	{
		player->x = rand() % ((WIDTH - 2) / 4) + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + 1;
	}
	else if (corner == 1)//右上
	{
		player->x = rand() % ((WIDTH - 2) / 4) + (WIDTH - 2) / 4 * 3 + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + 1;
	}
	else if (corner == 2)//左下
	{
		player->x = rand() % ((WIDTH - 2) / 4) + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + (HEIGHT - 2) / 4 * 3 + 1;
	}
	else if (corner == 3)//右下
	{
		player->x = rand() % ((WIDTH - 2) / 4) + (WIDTH - 2) / 4 * 3 + 1;
		player->y = rand() % ((HEIGHT - 2) / 4) + (HEIGHT - 2) / 4 * 3 + 1;
	}
	//敌群初始化
	for (int k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		enemies[k].ch = '#';
		//先全设为-1
		enemies[k].x = -1;
		enemies[k].y = -1;
		enemies[k].last_x = -1;
		enemies[k].last_y = -1;
		enemies[k].hitWallStep = 0;
		enemies[k].xyFlag = 0;
		enemies[k].trappedTime = 0;
	}
	//不重复位置地生成敌群
	//在另外三个角的区域内生成
	int enemy_corner;//每个敌人在哪个对角区域内生成
	int temp_x, temp_y,reFlag = 1;
	for (int k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		//抽到另外三个对角中的一个对角
		//该敌人就决定在那里生成
		do {
			enemy_corner = rand() % 4;
		} while (enemy_corner == corner);
		reFlag = 1;
		while (reFlag)//重抽
		{
			if (enemy_corner == 0)//左上
			{
				temp_x = rand() % ((WIDTH - 2) / 4) + 1;
				temp_y = rand() % ((HEIGHT - 2) / 4) + 1;
			}
			else if (enemy_corner == 1)//右上
			{
				temp_x = rand() % ((WIDTH - 2) / 4) + (WIDTH - 2) / 4 * 3 + 1;
				temp_y = rand() % ((HEIGHT - 2) / 4) + 1;
			}
			else if (enemy_corner == 2)//左下
			{
				temp_x = rand() % ((WIDTH - 2) / 4) + 1;
				temp_y = rand() % ((HEIGHT - 2) / 4) + (HEIGHT - 2) / 4 * 3 + 1;
			}
			else if (enemy_corner == 3)//右下
			{
				temp_x = rand() % ((WIDTH - 2) / 4) + (WIDTH - 2) / 4 * 3 + 1;
				temp_y = rand() % ((HEIGHT - 2) / 4) + (HEIGHT - 2) / 4 * 3 + 1;
			}
			//抽完默认reFlag为0
			reFlag = 0;
			for (int i = 0; i < ENEMY_NUM * info->difficulty; i++)
			{
				if (k == i)continue;
				if ((temp_x == enemies[i].x) && (temp_y == enemies[i].y))
				{
					//遇见重复的就重抽
					reFlag = 1;
					break;
				}
			}
		}
		enemies[k].x = temp_x;
		enemies[k].y = temp_y;
		//为每个敌人随机生成其反应能力数值，增加敌人多样性
		enemies[k].reflectTime = rand() % 9 - 4 + ENEMY_REFRESH_INTERVAL;//[-10,10] ∩ Z + ave
		//enemies[k].reflectTime = 8;
		//计数器清零
		enemies[k].prepareDegree = 0;
	}

	//生成障碍物
	//横向生成，中央上部下部各两条；左右端各一"条"
	//纵向生成，中央左部右部各一条；上下端各一"条"
	int x_length, y_length;
	x_length = rand() % (WIDTH / 8) - WIDTH / 16 + WIDTH / 8;
	y_length = rand() % (HEIGHT / 8) - HEIGHT / 16 + HEIGHT / 10;
	//生成位置
	int y_fix, x_fix,
		x_start,y_start;
	int u;
	//横的左上右上左下右下
	y_fix = rand() % (HEIGHT / 6) - HEIGHT / 12 + HEIGHT / 4;
	x_start = rand() % (WIDTH / 6) - WIDTH / 12 + WIDTH / 7 * 2 - x_length / 2;
	for (u = 0; u < x_length; u++)
	{
		obstacles[u].x = x_start++;
		obstacles[u].y = y_fix;
		obstacles[u].ch = '-';
	}
	y_fix = rand() % (HEIGHT / 6) - HEIGHT / 12 + HEIGHT / 4;
	x_start = rand() % (WIDTH / 6) - WIDTH / 12 + WIDTH / 7 * 5 - x_length / 2;
	for (; u < x_length * 2; u++)
	{
		obstacles[u].x = x_start++;
		obstacles[u].y = y_fix;
		obstacles[u].ch = '-';
	}
	y_fix = rand() % (HEIGHT / 6) - HEIGHT / 12 + HEIGHT / 4 * 3;
	x_start = rand() % (WIDTH / 6) - WIDTH / 12 + WIDTH / 7 * 2 - x_length / 2;
	for (; u < x_length * 3; u++)
	{
		obstacles[u].x = x_start++;
		obstacles[u].y = y_fix;
		obstacles[u].ch = '-';
	}
	y_fix = rand() % (HEIGHT / 6) - HEIGHT / 12 + HEIGHT / 4 * 3;
	x_start = rand() % (WIDTH / 6) - WIDTH / 12 + WIDTH / 7 * 5 - x_length / 2;
	for (; u < x_length * 4; u++)
	{
		obstacles[u].x = x_start++;
		obstacles[u].y = y_fix;
		obstacles[u].ch = '-';
	}
	//纵的左和右
	x_fix = rand() % (WIDTH / 9) - WIDTH / 18 + WIDTH / 5;
	y_start = rand() % (HEIGHT / 7) - HEIGHT / 14 + HEIGHT / 2 - y_length / 2;
	for (; u < x_length * 4 + y_length; u++)
	{
		obstacles[u].x = x_fix;
		obstacles[u].y = y_start++;
		obstacles[u].ch = '|';
	}
	x_fix = rand() % (WIDTH / 9) - WIDTH / 18 + WIDTH / 5 * 4;
	y_start = rand() % (HEIGHT / 7) - HEIGHT / 14 + HEIGHT / 2 - y_length / 2;
	for (; u < x_length * 4 + y_length * 2; u++)
	{
		obstacles[u].x = x_fix;
		obstacles[u].y = y_start++;
		obstacles[u].ch = '|';
	}
	//从一端开始，向内延伸是错落分布
	//左右两端
	int tu = u;
	x_start = 1;
	for (; u < tu + x_length * 2; u++)
	{
		y_fix = rand() % (HEIGHT / 2) - HEIGHT / 4 + HEIGHT / 2;
		obstacles[u].x = x_start++;
		obstacles[u].y = y_fix;
		obstacles[u].ch = 101;
	}
	x_start = WIDTH - 2;
	for (; u < tu + x_length * 4; u++)
	{
		y_fix = rand() % (HEIGHT / 2) - HEIGHT / 4 + HEIGHT / 2;
		obstacles[u].x = x_start--;
		obstacles[u].y = y_fix;
		obstacles[u].ch = 101;
	}
	//上下两端
	y_start = 1;
	for (; u < tu + x_length * 4 + y_length / 2 * 7; u++)
	{
		x_fix = rand() % (WIDTH / 2) - WIDTH / 4 + WIDTH / 2;
		obstacles[u].x = x_fix;
		obstacles[u].y = y_start++;
		obstacles[u].ch = 101;
	}
	y_start = HEIGHT - 2;
	for (; u < tu + x_length * 4 + y_length / 2 * 14; u++)
	{
		x_fix = rand() % (WIDTH / 2) - WIDTH / 4 + WIDTH / 2;
		obstacles[u].x = x_fix;
		obstacles[u].y = y_start--;
		obstacles[u].ch = 101;
	}
	int j = u;
	//中央区域随机生成x_length + y_length个“草丛”
	double span = (rand() % 3000 - 1500 + 2000) * 1.0 / 1000;//span为草丛标准覆盖范围的倍数：0.5~3.5倍
	for (; u < j + x_length + y_length; u++)
	{
		obstacles[u].x = (int)((rand() % (WIDTH / 5) - WIDTH / 10) * span + WIDTH / 2);
		obstacles[u].y = (int)((rand() % (HEIGHT / 4) - HEIGHT / 8) * span + HEIGHT / 2);
		obstacles[u].ch = 100;//以'▢：U+25A2'显示
	}
	//检测障碍物之间是否有坐标重复，若有，则将重复的其中一个障碍物用最后一个元素替代，并将总数减一
	int w, sub = 0, i;
	for (w = 0; w < u - sub; w++)
	{
		for (i = 0; i < u - sub; i++)
		{
			if (w == i)continue;
			if ((obstacles[w].x == obstacles[i].x) && (obstacles[w].y == obstacles[i].y))
			{
				sub++;
				obstacles[w] = obstacles[u - sub];
			}

		}
	}
	//将障碍物的实际数量记录在全局变量中
	obstacleNum = u - sub;
	for (int i = 0; i < obstacleNum; i++)
	{
		obstacles[i].isDestroyed = 0;
		obstacles[i].removeFlag = 0;
	}

	//子弹队列的头尾指针
	info->bulletsQueueHeadPtr = 0;
	info->bulletsQueueTailPtr = 0;
}
//清屏
void clearScreen()
{
	gotoxy(0, 0);
	for (int j = 0; j < HEIGHT + 1; j++)
	{
		for (int i = 0; i < WIDTH + 4; i++)
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
	gotoxy(WIDTH / 2 - 2, 5);
	printf("||");
	gotoxy(WIDTH / 2 - 2, 6);
	printf("||");
	//显示开始游戏的条件
	gotoxy(WIDTH / 2 - 7, 7);
	printf(">>>按空格键开始游戏<<<");
	//显示其他详情
	gotoxy(WIDTH / 2 - 2, 8);
	printf("||");
	gotoxy(WIDTH / 2 - 2, 9);
	printf("||");
	gotoxy(WIDTH / 2 + 32, 9);
	printf("/     /");
	gotoxy(WIDTH / 2 - 12, 10);
	printf("你将扮演一名求生的man，躲避多个敌人的追击  /");
	gotoxy(WIDTH / 2 - 12, 11);
	printf("坚持一百秒，你就赢了                                   -      --> >");
	gotoxy(WIDTH / 2 - 12, 12);
	printf("而中途碰到敌人，游戏结束并显示你坚持的秒数 \\");
	gotoxy(WIDTH / 2 + 32, 13);
	printf("\\     \\");
	gotoxy(WIDTH / 2 - 12, 13);
	printf("加油，真正的man!");	
	gotoxy(WIDTH / 2 - 3, 14);
	printf("|||||");
	gotoxy(WIDTH / 2 - 3, 15);
	printf("|||||");
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
	system("cls");
	//玩家输入要挑战的游戏难度
	gotoxy(WIDTH / 2 - 10, 7);
	printf("请输入你要挑战的游戏难度");
	gotoxy(WIDTH / 2 - 10, 8);
	printf("难度范围从1到9，数值越大敌人越多");
	//难度获取
	char diff;
	while (1)
	{
		diff = _getch();
		if ((diff >= '1') && (diff <= '9'))
		{
			info->difficulty = diff - '0';
			if (info->difficulty <= 3)	trappedTime = 5;
			else if (info->difficulty <= 6)	trappedTime = 7;
			else if (info->difficulty <= 9)	trappedTime = 9;
			//清屏
			clearScreen();
			return;
		}
		Sleep(10);
	}
}
//显示
void show(Info* info,Player* player,Enemy enemies[],Obstacle obstacles[],Bullet bullets[])
{
	gotoxy(0, 0);
	//显示边界、玩家和敌群
	static int j, i;
	for (j = 0; j < HEIGHT; j++)
	{
		for (i = 0; i < WIDTH; i++)
		{
			if ((j == 0) || (j == HEIGHT - 1))//横向边框
			{
				gotoxy(i, j);
				printf("-");
			}
			else if ((i == 0) || (i == WIDTH - 1))//纵向边框
			{
				gotoxy(i, j);
				printf("|");
			}
		}
	}
	//显示玩家
	if ((player->last_x != player->x) || (player->last_y != player->y))
	{
		gotoxy(player->last_x, player->last_y);
		printf(" ");

		gotoxy(player->x, player->y);
		printf("%c", player->ch);
	}
	//显示每个敌人
	static int k;
	for (k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		if ((enemies[k].last_x != enemies[k].x) || (enemies[k].last_y != enemies[k].y))
		{
			gotoxy(enemies[k].last_x, enemies[k].last_y);
			printf(" ");

			gotoxy(enemies[k].x, enemies[k].y);
			if (enemies[k].trappedTime == 0)	printf("%c", enemies[k].ch);
			else if (enemies[k].trappedTime > 0)	printf("%d",enemies[k].trappedTime);
		}
	}
	//显示静止障碍物
	static int u;
	for (u = 0; u < obstacleNum; u++)
	{
		if (obstacles[u].isDestroyed == 0)
		{
			gotoxy(obstacles[u].x, obstacles[u].y);
			if (obstacles[u].ch == 100)//草丛
			{
				printf("\u25A1");
			}
			else if (obstacles[u].ch == 101)//乱石
			{
				printf("\u25A0");
			}
			else
			{
				printf("%c", obstacles[u].ch);//墙
			}
		}
		else if ((obstacles[u].isDestroyed == 1) && (obstacles[u].removeFlag == 0))
		{
			gotoxy(obstacles[u].x, obstacles[u].y);
			printf(" ");
			obstacles[u].removeFlag = 1;
		}
	}
	//显示坚持的秒数
	gotoxy(0, HEIGHT);
	printf("坚持的秒数：%lu.%lu s         \n", info->ms / 1000,info->ms % 1000);
	//显示当前难度
	gotoxy(WIDTH + 3, HEIGHT / 2 - 5);
	printf("当");
	gotoxy(WIDTH + 3, HEIGHT / 2 - 4);
	printf("前");
	gotoxy(WIDTH + 3, HEIGHT / 2 - 3);
	printf("难");
	gotoxy(WIDTH + 3, HEIGHT / 2 - 2);
	printf("度");
	gotoxy(WIDTH + 3, HEIGHT / 2);
	printf("|");
	gotoxy(WIDTH + 3, HEIGHT / 2 + 1);
	printf("V");
	gotoxy(WIDTH + 3, HEIGHT / 2 + 3);
	printf("%d",info->difficulty);

	//显示子弹
	static int t;
	for (t = info->bulletsQueueHeadPtr; t < info->bulletsQueueTailPtr; t++)
	{
		if (bullets[t].isDestroyed == 0)
		{
			gotoxy(bullets[t].last_x, bullets[t].last_y);
			printf(" ");
			gotoxy(bullets[t].x, bullets[t].y);
			if (bullets[t].ch == 1)//圆形子弹
			{
				printf("\u25CF");
			}
		}
	}
}
//与用户输入无关的更新
void updateWithoutInput(Info* info,Player* player,Enemy enemies[],Obstacle obstacles[],Bullet bullets[])
{
	static int temp_x, temp_y, reFlag = 1,randFlag = 0,initRandFlagRate = 0,account = 0,speedup = 0;
	static int k,t,ti,pre_x,pre_y,x_flag,y_flag;
	static int i;
	static clock_t curTime;
	curTime = clock();
	//每个敌人的位置更新
	for (k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		if (enemies[k].trappedTime == 0)
		{
			if (enemies[k].prepareDegree < enemies[k].reflectTime)
			{
				enemies[k].prepareDegree++;
			}
			else if (enemies[k].prepareDegree == enemies[k].reflectTime)
			{
				if (enemies[k].hitWallStep == 0)
				{
					pre_x = enemies[k].last_x;
					pre_y = enemies[k].last_y;
					//重置reFlag
					reFlag = 1;
					//按照坐标差计算行动方向概率
					initRandFlagRate = rand() % 10000;//0 ~ 9999的整数
					account = (int)(fabs(player->x - enemies[k].x) * 10000 / (fabs(player->x - enemies[k].x) + fabs(player->y - enemies[k].y)));
					//判断initRandFlagRate是否落在0 ~ account内
					//若是，则尝试水平方向追逐
					if (initRandFlagRate < account)
					{
						randFlag = 0;
					}
					//否则，竖直方向追逐
					else
					{
						randFlag = 1;
					}
					//speedup决定敌人是否对角加速
					speedup = rand() % 10000;
					if (speedup < DIAG_SPEEDUP_RATE)
					{
						randFlag = 2;
					}
					//敌人尝试靠近玩家
					//还要保证刷新后的位置不与其他敌人重复
					if (reFlag == 1)
					{
						//迫近行动
						//第一次生成
						//不使用对角加速的情况
						if (randFlag != 2)
						{
							if (player->x == enemies[k].x)//一条竖线上，不应离开竖线
							{
								//竖直方向尝试追逐
								if (player->y > enemies[k].y)
								{
									temp_y = enemies[k].y + 1;
								}
								else if (player->y < enemies[k].y)
								{
									temp_y = enemies[k].y - 1;
								}
								temp_x = enemies[k].x;
							}
							else if (player->y == enemies[k].y)//一条横线上，不应离开横线
							{
								//水平方向尝试追逐
								if (player->x > enemies[k].x)
								{
									temp_x = enemies[k].x + 1;
								}
								else if (player->x < enemies[k].x)
								{
									temp_x = enemies[k].x - 1;
								}
								temp_y = enemies[k].y;
							}
							else//不在一条线上
							{
								if (randFlag == 0)//决定水平方向追逐
								{
									if (player->x > enemies[k].x)
									{
										temp_x = enemies[k].x + 1;
									}
									else if (player->x < enemies[k].x)
									{
										temp_x = enemies[k].x - 1;
									}
									temp_y = enemies[k].y;
								}
								else if (randFlag == 1)//决定竖直方向追逐
								{
									if (player->y > enemies[k].y)
									{
										temp_y = enemies[k].y + 1;
									}
									else if (player->y < enemies[k].y)
									{
										temp_y = enemies[k].y - 1;
									}
									temp_x = enemies[k].x;
								}
							}
						}
						//使用对角加速的情况
						//无论是否在一条线上，都尝试对角靠近玩家
						//对角加速需要考虑撞墙的情况
						else
						{
							if (player->x == enemies[k].x)//一条竖线上
							{
								if (player->y > enemies[k].y)
								{
									temp_y = enemies[k].y + 1;
								}
								else if (player->y < enemies[k].y)
								{
									temp_y = enemies[k].y - 1;
								}
								//判断是否可能撞墙并采取相应处理
								if (enemies[k].x == 1)
								{
									temp_x = enemies[k].x + 1;
								}
								else if (enemies[k].x == WIDTH - 2)
								{
									temp_x = enemies[k].x - 1;
								}
								else
								{
									temp_x = rand() % 2 * 2 - 1 + enemies[k].x;
								}
							}
							else if (player->y == enemies[k].y)//一条横线上
							{
								if (player->x > enemies[k].x)
								{
									temp_x = enemies[k].x + 1;
								}
								else if (player->x < enemies[k].x)
								{
									temp_x = enemies[k].x - 1;
								}
								//判断是否可能撞墙并采取相应处理
								if (enemies[k].y == 1)
								{
									temp_y = enemies[k].y + 1;
								}
								else if (enemies[k].y == HEIGHT - 2)
								{
									temp_y = enemies[k].y - 1;
								}
								else
								{
									temp_y = rand() % 2 * 2 - 1 + enemies[k].y;
								}
							}
							else //不在一条线上
							{
								if (player->x > enemies[k].x)
								{
									temp_x = enemies[k].x + 1;
								}
								else if (player->x < enemies[k].x)
								{
									temp_x = enemies[k].x - 1;
								}
								if (player->y > enemies[k].y)
								{
									temp_y = enemies[k].y + 1;
								}
								else if (player->y < enemies[k].y)
								{
									temp_y = enemies[k].y - 1;
								}
							}
						}
					}

					//判断是否有敌人坐标重复
					for (i = 0; i < ENEMY_NUM * info->difficulty; i++)
					{
						if (k == i)continue;//自己不检测
						if ((temp_x == enemies[i].x) && (temp_y == enemies[i].y))
						{
							reFlag = 2;
							break;
						}
					}
					//通过则进行障碍物检测
					if (reFlag == 1)
					{
						//或碰障碍物
						for (i = 0; i < obstacleNum; i++)
						{
							if (obstacles[i].ch == 100)//敌人可以自如地穿过中央的繁茂草丛
								continue;
							if ((temp_x == obstacles[i].x) && (temp_y == obstacles[i].y))//敌人不能穿过围墙
							{
								reFlag = 4;
								break;
							}
						}
					}

					//如果不重复且不越界
					//采取一号生成方案
					if ((reFlag == 1) &&
						!((temp_x < 1) || (temp_x > WIDTH - 2) || (temp_y < 1) || (temp_y > HEIGHT - 2)))
					{
						enemies[k].last_x = enemies[k].x;
						enemies[k].last_y = enemies[k].y;
						enemies[k].x = temp_x;
						enemies[k].y = temp_y;
						enemies[k].prepareDegree = 0;
						continue;
					}

					//一号生成出现敌人位置重合
					//就开始二号生成
					if (reFlag == 2)
					{
						static int oppo;
						oppo = 4;
						do
						{
							//试探行动（确保不撞墙）（只能横纵走）
							static int rowOrCol = 0;//0横走1纵走
							if (rowOrCol == 0)
							{
								//x坐标第二次生成
								temp_x = rand() % 2 * 2 - 1 + enemies[k].x;
								//y坐标第二次生成
								temp_y = enemies[k].y;
							}
							else
							{
								//x坐标第二次生成
								temp_x = enemies[k].x;
								//y坐标第二次生成
								temp_y = rand() % 2 * 2 - 1 + enemies[k].y;
							}
							//判断是否有坐标重复
							for (i = 0; i < ENEMY_NUM * info->difficulty; i++)
							{
								if (k == i)continue;//自己不检测
								if ((temp_x == enemies[i].x) && (temp_y == enemies[i].y))
								{
									reFlag = 3;
									break;
								}
							}
							if (reFlag == 3)oppo = 0;
							else oppo--;
						} while (((temp_x < 1) || (temp_x > WIDTH - 2) || (temp_y < 1) || (temp_y > HEIGHT - 2)) || (oppo));
					}
					//敌人位置检测通过
					//进行障碍物检测
					if (reFlag != 3)
					{
						//或碰障碍物
						for (i = 0; i < obstacleNum; i++)
						{
							if (obstacles[i].ch == 100)//敌人可以自如地穿过中央的繁茂草丛
								continue;
							if ((temp_x == obstacles[i].x) && (temp_y == obstacles[i].y))//敌人不能穿过围墙
							{
								reFlag = 4;
								break;
							}
						}
					}

					//如果不重复且不越界
					//采取二号生成方案
					if ((reFlag == 2) &&
						!((temp_x < 1) || (temp_x > WIDTH - 2) || (temp_y < 1) || (temp_y > HEIGHT - 2)))
					{
						enemies[k].last_x = enemies[k].x;
						enemies[k].last_y = enemies[k].y;
						enemies[k].x = temp_x;
						enemies[k].y = temp_y;
						enemies[k].prepareDegree = 0;
						continue;
					}

					//如果重复或越界：
					//1.二号生成出现敌人位置重复，
					// 则转三号生成（不行动）
					if (reFlag == 3)
					{
						//本次不行动
						//第三次生成（一类，敌人拥挤时，模拟敌人观望）
						enemies[k].last_x = enemies[k].x;
						enemies[k].last_y = enemies[k].y;
						enemies[k].prepareDegree = 0;
						continue;
					}
					//2.一或二号检测到碰障碍物，
					// 则转四号生成（绕墙行动1）
					if (reFlag == 4)
					{
						//绕墙行动
						//（二类，敌人碰到墙时，模拟敌人沿墙摸索）
						//判断四方向中朝玩家的两个分方向都有没有障碍物
						//距离较远者
						x_flag = 0;
						y_flag = 0;
						if (fabs(player->x - enemies[k].x) > fabs(player->y - enemies[k].y))
						{
							x_flag = 1;
							if (player->x > enemies[k].x)
							{
								temp_x = enemies[k].x + 1;
								temp_y = enemies[k].y;
							}
							else
							{
								temp_x = enemies[k].x - 1;
								temp_y = enemies[k].y;
							}
						}
						else
						{
							y_flag = 1;
							if (player->y > enemies[k].y)
							{
								temp_x = enemies[k].x;
								temp_y = enemies[k].y + 1;
							}
							else
							{
								temp_x = enemies[k].x;
								temp_y = enemies[k].y - 1;
							}
						}
						//碰障碍物判断
						for (i = 0; i < obstacleNum; i++)
						{
							if (obstacles[i].ch == 100)//敌人可以自如地穿过中央的繁茂草丛
								continue;

							if ((temp_x == obstacles[i].x) && (temp_y == obstacles[i].y))
							{
								reFlag = 5;
								break;
							}
						}
						//碰敌判断
						if (reFlag != 5)
						{
							for (i = 0; i < ENEMY_NUM * info->difficulty; i++)
							{
								if (k == i)continue;//自己不检测
								if ((temp_x == enemies[i].x) && (temp_y == enemies[i].y))
								{
									reFlag = 5;
									break;
								}
							}
						}
					}

					//四号生成没重复且不越界
					if ((reFlag == 4) &&
						!((temp_x < 1) || (temp_x > WIDTH - 2) || (temp_y < 1) || (temp_y > HEIGHT - 2)))
					{
						enemies[k].last_x = enemies[k].x;
						enemies[k].last_y = enemies[k].y;
						enemies[k].x = temp_x;
						enemies[k].y = temp_y;
						enemies[k].prepareDegree = 0;
						continue;
					}
					//四号生成有重复
					//转五号生成（绕墙行动2）
					if (reFlag == 5)
					{
						if (x_flag)
						{
							if (player->y > enemies[k].y)
							{
								temp_x = enemies[k].x;
								temp_y = enemies[k].y + 1;
							}
							else
							{
								temp_x = enemies[k].x;
								temp_y = enemies[k].y - 1;
							}
						}
						else if (y_flag)
						{
							if (player->x > enemies[k].x)
							{
								temp_x = enemies[k].x + 1;
								temp_y = enemies[k].y;
							}
							else
							{
								temp_x = enemies[k].x - 1;
								temp_y = enemies[k].y;
							}
						}
						//碰障碍物判断
						for (i = 0; i < obstacleNum; i++)
						{
							if (obstacles[i].ch == 100)//敌人可以自如地穿过中央的繁茂草丛
								continue;

							if ((temp_x == obstacles[i].x) && (temp_y == obstacles[i].y))
							{
								reFlag = 6;
								if (temp_x > enemies[k].x)enemies[k].xyFlag = 1;
								else if (temp_x < enemies[k].x)enemies[k].xyFlag = 3;
								else if (temp_y > enemies[k].y)enemies[k].xyFlag = 2;
								else if (temp_y < enemies[k].y)enemies[k].xyFlag = 4;
								break;
							}
						}
						//碰敌判断
						if (reFlag != 6)
						{
							for (i = 0; i < ENEMY_NUM * info->difficulty; i++)
							{
								if (k == i)continue;//自己不检测
								if ((temp_x == enemies[i].x) && (temp_y == enemies[i].y))
								{
									reFlag = 6;
									if (temp_x > enemies[k].x)enemies[k].xyFlag = 1;
									else if (temp_x < enemies[k].x)enemies[k].xyFlag = 3;
									else if (temp_y > enemies[k].y)enemies[k].xyFlag = 2;
									else if (temp_y < enemies[k].y)enemies[k].xyFlag = 4;
									break;
								}
							}
						}
					}
					//五号生成没重复且不越界
					if ((reFlag == 5) &&
						!((temp_x < 1) || (temp_x > WIDTH - 2) || (temp_y < 1) || (temp_y > HEIGHT - 2)))
					{
						enemies[k].last_x = enemies[k].x;
						enemies[k].last_y = enemies[k].y;
						enemies[k].x = temp_x;
						enemies[k].y = temp_y;
						enemies[k].prepareDegree = 0;
						continue;
					}
					//五号生成还重复，
					//则转六号生成（朝上一个方向的反方向走n步）
					if (reFlag == 6)
					{
						//尝试朝上一个方向的反方向走n步
						enemies[k].hitWallStep = rand() % 7 - 3 + 7;//n
						//方向取反
						enemies[k].xyFlag = (enemies[k].xyFlag + 2) % 4;
						////回退行动 尝试3
						//temp_x = pre_x;
						//temp_y = pre_y;
						////碰敌判断
						//for (i = 0; i < ENEMY_NUM * info->difficulty; i++)
						//{
						//	if (k == i)continue;//自己不检测
						//	if ((temp_x == enemies[i].x) && (temp_y == enemies[i].y))
						//	{
						//		reFlag = 7;
						//		break;
						//	}
						//}
					}
					goto GoFurther;
				}
				////尝试3没重复
				//if (reFlag == 6)
				//{
				//	enemies[k].last_x = enemies[k].x;
				//	enemies[k].last_y = enemies[k].y;
				//	enemies[k].x = temp_x;
				//	enemies[k].y = temp_y;
				//	enemies[k].prepareDegree = 0;
				//	continue;
				//}
				////尝试3还重复
				//if (reFlag == 7)
				//{
				//	enemies[k].last_x = enemies[k].x;
				//	enemies[k].last_y = enemies[k].y;
				//	enemies[k].prepareDegree = 0;
				//	continue;
				//}
				else
				{
				GoFurther:
					//下一个位置
					if (enemies[k].xyFlag == 0)//上
					{
						temp_x = enemies[k].x;
						temp_y = enemies[k].y - 1;
					}
					else if (enemies[k].xyFlag == 1)
					{
						temp_x = enemies[k].x + 1;
						temp_y = enemies[k].y;
					}
					else if (enemies[k].xyFlag == 2)
					{
						temp_x = enemies[k].x;
						temp_y = enemies[k].y + 1;
					}
					else if (enemies[k].xyFlag == 3)
					{
						temp_x = enemies[k].x - 1;
						temp_y = enemies[k].y;
					}

					//检测敌人位置重复情况和障碍物情况
					for (i = 0; i < ENEMY_NUM * info->difficulty; i++)
					{
						if (k == i)continue;//自己不检测
						if ((temp_x == enemies[i].x) && (temp_y == enemies[i].y))
						{
							reFlag = -1;
							break;
						}
					}
					if (reFlag != -1)
					{
						for (i = 0; i < obstacleNum; i++)
						{
							if (obstacles[i].ch == 100)//敌人可以自如地穿过中央的繁茂草丛
								continue;

							if ((temp_x == obstacles[i].x) && (temp_y == obstacles[i].y))
							{
								reFlag = -1;
								break;
							}
						}
					}

					//若有重复或越界，就不走
					if ((reFlag == -1) ||
						((temp_x < 1) || (temp_x > WIDTH - 2) || (temp_y < 1) || (temp_y > HEIGHT - 2)))
					{
						enemies[k].last_x = enemies[k].x;
						enemies[k].last_y = enemies[k].y;
						enemies[k].hitWallStep = 0;
						enemies[k].prepareDegree = 0;
						continue;
					}
					//没重复且没越界
					else
					{
						enemies[k].last_x = enemies[k].x;
						enemies[k].last_y = enemies[k].y;
						enemies[k].x = temp_x;
						enemies[k].y = temp_y;
						enemies[k].hitWallStep--;
						enemies[k].prepareDegree = 0;
						continue;
					}
				}

			}
		}
		else if (enemies[k].trappedTime > 0)
		{
			//每过一秒刷新trappedTime
			if ((1.0 * (curTime - enemies[k].startTime) / CLOCKS_PER_SEC) >= 1.0)
			{
				enemies[k].startTime = clock();
				enemies[k].trappedTime--;
			}
		}
	}
	//子弹的位置更新
	//每次当前位置检测和边缘检测
	for (t = info->bulletsQueueHeadPtr; t < info->bulletsQueueTailPtr; t++)
	{
		if (bullets[t].isDestroyed == 0)
		{
			//边缘检测 &
			if ((bullets[t].x < 1) || (bullets[t].x > WIDTH - 2) ||
				(bullets[t].y < 1) || (bullets[t].y > HEIGHT - 2))
					bullets[t].isDestroyed = 1;
			//敌人位置检测
			for (ti = 0; ti < ENEMY_NUM * info->difficulty; ti++)
			{
				if ((bullets[t].x == enemies[ti].x) && (bullets[t].y == enemies[ti].y))
				{
					bullets[t].isDestroyed = 1;
					enemies[ti].trappedTime = trappedTime;
					enemies[ti].prepareDegree= 0;
					enemies[ti].startTime = clock();
					break;
				}
			}
			//障碍物检测
			for (ti = 0; ti < obstacleNum; ti++)
			{
				if ((bullets[t].x == obstacles[ti].x) && (bullets[t].y == obstacles[ti].y) &&
					(obstacles[ti].isDestroyed == 0))
				{
					bullets[t].isDestroyed = 1;
					if (obstacles[ti].ch == 100)//草丛可被打掉
					{
						obstacles[ti].isDestroyed = 1;
					}
					break;
				}
			}
			//子弹撞毁处理
			if (bullets[t].isDestroyed) 
			{
				if (t == info->bulletsQueueHeadPtr) info->bulletsQueueHeadPtr++;
				continue;
			}
			//子弹飞行
			if (bullets[t].direction == 0)//上
			{
				bullets[t].last_x = bullets[t].x;
				bullets[t].last_y = bullets[t].y;
				bullets[t].y--;
			}
			else if (bullets[t].direction == 1)//右
			{
				bullets[t].last_x = bullets[t].x;
				bullets[t].last_y = bullets[t].y;
				bullets[t].x++;
			}
			else if (bullets[t].direction == 2)//下
			{
				bullets[t].last_x = bullets[t].x;
				bullets[t].last_y = bullets[t].y;
				bullets[t].y++;
			}
			else if (bullets[t].direction == 3)//左
			{
				bullets[t].last_x = bullets[t].x;
				bullets[t].last_y = bullets[t].y;
				bullets[t].x--;
			}
		}
	}


	//判断玩家与敌人是否重合（坐标重合游戏结束）
	for (k = 0; k < ENEMY_NUM * info->difficulty; k++)
	{
		if ((player->x == enemies[k].x) && (player->y == enemies[k].y))
		{
			info->failFlag = 1;
			break;
		}
	}
}
//与用户输入有关的更新
void updateWithInput(Info* info,Player* player,Obstacle obstacles[],Bullet bullets[])
{
	static char ch,obsFlag = 1;//默认可通过1
	static int u;
	if (_kbhit())//当检测到输入
	{
		ch = _getch();
		//玩家移动
		if ((ch == 'w') && (player->y > 1))//向上移动
		{
			player->direction = 0;
			obsFlag = 1;
			//检测是否有障碍物阻挡前进
			for (u = 0; u < obstacleNum; u++)
			{
				if (obstacles[u].isDestroyed == 1)	continue;
				if ((player->x == obstacles[u].x) && (player->y - 1 == obstacles[u].y))
					obsFlag = 0;//本次不能通过
			}
			if (obsFlag)//能通过才能前进
			{
				player->last_x = player->x;
				player->last_y = player->y;
				player->y--;
			}
		}
		else if ((ch == 'a') && (player->x > 1))//向左移动
		{
			player->direction = 3;
			obsFlag = 1;
			//检测是否有障碍物阻挡前进
			for (u = 0; u < obstacleNum; u++)
			{
				if (obstacles[u].isDestroyed == 1)	continue;
				if ((player->x - 1 == obstacles[u].x) && (player->y == obstacles[u].y))
					obsFlag = 0;//本次不能通过
			}
			if (obsFlag)//能通过才能前进
			{
				player->last_x = player->x;
				player->last_y = player->y;
				player->x--;
			}
		}
		else if ((ch == 's') && (player->y < HEIGHT - 2))//向下移动
		{
			player->direction = 2;
			obsFlag = 1;
			//检测是否有障碍物阻挡前进
			for (u = 0; u < obstacleNum; u++)
			{
				if (obstacles[u].isDestroyed == 1)	continue;
				if ((player->x == obstacles[u].x) && (player->y + 1 == obstacles[u].y))
					obsFlag = 0;//本次不能通过
			}
			if (obsFlag)//能通过才能前进
			{
				player->last_x = player->x;
				player->last_y = player->y;
				player->y++;
			}
		}
		else if ((ch == 'd') && (player->x < WIDTH - 2))//向右移动
		{
			player->direction = 1;
			obsFlag = 1;
			//检测是否有障碍物阻挡前进
			for (u = 0; u < obstacleNum; u++)
			{
				if (obstacles[u].isDestroyed == 1)	continue;
				if ((player->x + 1 == obstacles[u].x) && (player->y == obstacles[u].y))
					obsFlag = 0;//本次不能通过
			}
			if (obsFlag)//能通过才能前进
			{
				player->last_x = player->x;
				player->last_y = player->y;
				player->x++;
			}
		}
		//玩家发射子弹
		if (ch == 'j')
		{
			if (player->direction == 0)
			{
				bullets[info->bulletsQueueTailPtr].x = player->x;
				bullets[info->bulletsQueueTailPtr].y = player->y - 1;
				bullets[info->bulletsQueueTailPtr].direction = player->direction;
			}
			else if (player->direction == 1)
			{
				bullets[info->bulletsQueueTailPtr].x = player->x + 1;
				bullets[info->bulletsQueueTailPtr].y = player->y;
				bullets[info->bulletsQueueTailPtr].direction = player->direction;
			}
			else if (player->direction == 2)
			{
				bullets[info->bulletsQueueTailPtr].x = player->x;
				bullets[info->bulletsQueueTailPtr].y = player->y + 1;
				bullets[info->bulletsQueueTailPtr].direction = player->direction;
			}
			else if (player->direction == 3)
			{
				bullets[info->bulletsQueueTailPtr].x = player->x - 1;
				bullets[info->bulletsQueueTailPtr].y = player->y;
				bullets[info->bulletsQueueTailPtr].direction = player->direction;
			}

			bullets[info->bulletsQueueTailPtr].ch = 1;
			bullets[info->bulletsQueueTailPtr].isDestroyed = 0;
			info->bulletsQueueTailPtr++;
			if (info->bulletsQueueTailPtr > MAX_BULLET_NUM)	
			{
				info->bulletsQueueHeadPtr = 0;
				info->bulletsQueueTailPtr = 0;
			}
		}
	}
}
//游戏结束的处理（赢或输）
int Process(Info* info,Enemy enemies[],Obstacle obstacles[],Bullet bullets[])
{
	//敌群和障碍物动态数组内存的回收
	free(enemies);
	free(obstacles);
	free(bullets);

	//显示与玩家输入
	if (info->failFlag == 0)
	{
		gotoxy(20,HEIGHT);
		printf("     你赢了，真正的man!\n\n");
	}
	else if (info->failFlag == 1)
	{
		gotoxy(20, HEIGHT);
		printf("     你被敌人抓住了！\n\n");
	}
	printf("摁下空格键退出游戏<<<        >>>摁下r键再来一局<<<\n");
	char ch;
	while (1)
	{
		if (_kbhit())
		{
			ch = _getch();
			if (ch == ' ')
			{
				return 0;
			}
			else if (ch == 'r')
			{
				return 1;
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
Restart:
	HideCursor();
	Info info;
	Player player;
	Obstacle* obstacles = (Obstacle*)malloc(OBSTACLES_NUM * sizeof(Obstacle));//存储多个障碍物
	mainMenu(&info);
	Enemy* enemies = (Enemy*)malloc(ENEMY_NUM * info.difficulty * sizeof(Enemy));
	Bullet* bullets = (Bullet*)malloc(MAX_BULLET_NUM * sizeof(Bullet));
	startup(&info,&player,enemies,obstacles,bullets);
	clock_t startTime = clock();//程序开始时间
	while ((info.failFlag == 0) && (info.ms < 100 * 1000))
	{
		static clock_t currentTime,runLoopTime = DELAY_MS;
		if (runLoopTime < DELAY_MS)Sleep(DELAY_MS - runLoopTime);//锁帧 1000 / DELAY_MS 帧

		currentTime = clock();//程序当前时间
		info.ms = (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;  // 计算经过的毫秒数
		show(&info,&player,enemies,obstacles,bullets);

		updateWithoutInput(&info,&player,enemies,obstacles,bullets);
		updateWithInput(&info,&player,obstacles,bullets);
		Delay();

		runLoopTime = clock();//当次循环结束时间
		runLoopTime = (runLoopTime - currentTime) * 1000 / CLOCKS_PER_SEC;//计算当次循环耗时（ms）
	}
	int restart = Process(&info,enemies,obstacles,bullets);
	if (restart)
	{
		system("cls");
		goto Restart;
	}
	return 0;
}