//SweepBomb by Toverlight
//ver 1.0 released 24/2/9
//started 24/2/8
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <graphics.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")//已导入静态库，可写可不写

#define ROW 20//行
#define COL 30//列
#define NUM 50//雷的个数
#define SIZE 40//一行的宽度

int arr[ROW + 2][COL + 2] = { 0 };
IMAGE img[12];
int win = 0;

void GameInit()
{
	win = 0;
	//初始化数组
	for (int i = 1; i < ROW + 1; i++)
	{
		for (int j = 1; j < COL + 1; j++)
		{
			arr[i][j] = 0;
		}
	}

	//开始音乐
	mciSendString(L"open ./res/load2.mp3", 0, 0, 0);
	mciSendString(L"play ./res/magic2.mp3", 0, 0, 0);

	//设置随机数种子
	srand((unsigned int)time(NULL));

	//不重复地埋NUM个雷
	for (int k = 0; k < NUM; )
	{
		int r = rand() % ROW + 1;
		int c = rand() % COL + 1;
		if (arr[r][c] == 0)
		{
			arr[r][c] = -1;
			k++;
		}
	}
	//计算周围雷的个数
	for (int i = 1; i < ROW + 1; i++)
	{
		for (int j = 1; j < COL + 1; j++)
		{
			if (arr[i][j] == 0)
			{
				for (int x = i - 1; x < i + 2; x++)
				{
					for (int y = j - 1; y < j + 2; y++)
					{
						if (!(x == i && y == j) && arr[x][y] == -1)
						{
							arr[i][j]++;
						}
					}
				}
			}
		}
	}

	//加密格子
	for (int i = 1; i < ROW + 1; i++)
	{
		for (int j = 1; j < COL + 1; j++)
		{
			arr[i][j] += 20;
		}
	}
}
//绘制地图
void DrawMap()
{
	for (int i = 1; i < ROW + 1; i++)
	{
		for (int j = 1; j < COL + 1; j++)
		{
			//printf("%3d", arr[i][j]);
			if (arr[i][j] == -1)//显示雷
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[10]);
			}
			else if (arr[i][j] >= 0 && arr[i][j] <= 8)//显示数字
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[arr[i][j]]);
			}
			else if (arr[i][j] >= 19 && arr[i][j] <= 28)//显示加密块
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[9]);
			}
			else if (arr[i][j] > 30)//显示标记
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[11]);
			}
		}
		//printf("\n");
	}
}
//打开0
void UnfoldZero(int r, int c)
{
	arr[r][c] -= 20;//打开这个0
	++win;

	for (int i = r - 1; i < r + 2; i++)
	{
		for (int j = c - 1; j < c + 2; j++)
		{
			if (i >= 1 && i < ROW + 1 && j >= 1 && j < COL + 1)
			{
				//加密的格子中
				if (arr[i][j] == 20)//是0
				{
					UnfoldZero(i, j);
				}
				else if (arr[i][j] >= 21 && arr[i][j] <= 28)//不是0
				{
					arr[i][j] -= 20;
					++win;
				}
			}
		}
	}
}
//操作处理
int PlayGame()
{
	MOUSEMSG msg = { 0 };
	msg = GetMouseMsg();

	int n;
	if (msg.uMsg == WM_LBUTTONDOWN)//鼠标左键点下的消息
	{
		n = arr[msg.y / SIZE + 1][msg.x / SIZE + 1];//获取鼠标点击到的块的数据
		//1 - 8打开
		if (n >= 19 && n <= 28)
		{
			//0则打开一片
			//-1则结束游戏
			if (n == 20)//0
			{
				UnfoldZero(msg.y / SIZE + 1, msg.x / SIZE + 1);
				PlaySound(L"./res/Attack2.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			else if (n == 19)//雷
			{
				arr[msg.y / SIZE + 1][msg.x / SIZE + 1] -= 20;
				PlaySound(L"./res/Thunder9.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			else
			{
				arr[msg.y / SIZE + 1][msg.x / SIZE + 1] -= 20;
				PlaySound(L"./res/Cursor3.wav", NULL, SND_FILENAME | SND_ASYNC);
				++win;
			}

		}

	}
	else if (msg.uMsg == WM_RBUTTONDOWN)//鼠标右键点下的消息
	{
		n = arr[msg.y / SIZE + 1][msg.x / SIZE + 1];//获取鼠标点击到的块的数据
		if (n >= 19 && n <= 28)
		{
			arr[msg.y / SIZE + 1][msg.x / SIZE + 1] += 30;
			PlaySound(L"./res/Knock.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else if (n > 30)
		{
			arr[msg.y / SIZE + 1][msg.x / SIZE + 1] -= 30;
			PlaySound(L"./res/Knock.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
	}
	return arr[msg.y / SIZE + 1][msg.x / SIZE + 1];
}


int main()
{
	HWND hwnd = initgraph(COL * SIZE, ROW * SIZE);
	//加载图片
	loadimage(&img[0], L"./res/0.png", SIZE, SIZE);
	loadimage(&img[1], L"./res/1.png", SIZE, SIZE);
	loadimage(&img[2], L"./res/2.png", SIZE, SIZE);
	loadimage(&img[3], L"./res/3.png", SIZE, SIZE);
	loadimage(&img[4], L"./res/4.png", SIZE, SIZE);
	loadimage(&img[5], L"./res/5.png", SIZE, SIZE);
	loadimage(&img[6], L"./res/6.png", SIZE, SIZE);
	loadimage(&img[7], L"./res/7.png", SIZE, SIZE);
	loadimage(&img[8], L"./res/8.png", SIZE, SIZE);
	loadimage(&img[9], L"./res/20.png", SIZE, SIZE);
	loadimage(&img[10], L"./res/_1.png", SIZE, SIZE);
	loadimage(&img[11], L"./res/50.png", SIZE, SIZE);
	//游戏循环
	while(1)
	{
		GameInit();
		//一局游戏
		while (1)
		{
			DrawMap();
			if (PlayGame() == -1)
			{
				DrawMap();
				MessageBox(hwnd, L"你输了！", L"", MB_OK);
				break;
			}
			else if (ROW * COL - win == NUM)
			{
				DrawMap();
				MessageBox(hwnd, L"你赢了", L"", MB_OK);
				break;
			}
		}
		if (MessageBox(hwnd, L"再来一局？", L"", MB_RETRYCANCEL) == IDCANCEL)
		{
			break;
		}
		mciSendString(L"close ./res/magic2.mp3", 0, 0, 0);
	}
	return 0;
}