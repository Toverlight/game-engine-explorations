#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>
#include <conio.h>
#include <Windows.h>
#define PI 3.14
int  main()
{
	initgraph(800, 600);//初始化窗口
	HWND hwnd = GetHWnd();  // 获取窗口句柄
	SetWindowText(hwnd, TEXT("My EasyX Window"));  // 设置窗口标题
	setbkcolor(WHITE);//背景颜色
	cleardevice();//清屏
	setorigin(400, 300);//设置原点
	setaspectratio(1, -1);//设置x,y轴的正方向

	setcolor(BLUE);//设置画笔颜色为蓝色
	circle(0, 0, 100);//画圆
	line(-200, 200, 200, -200);//画直线

	setcolor(BLACK);
	int x, y;
	for (int i = 0; i < 1000; i++)//随机在整个窗口打印1000个散点
	{
		x = rand() % (800 + 1) - 400;
		y = rand() % (600 + 1) - 300;
		putpixel(x, y, BLACK);//在(x,y)处打印一个点
	}

	setcolor(RED);
	rectangle(-200, 100, 200, -100);//绘制矩形

	setcolor(GREEN);
	ellipse(-200, 100, 200, -100);//绘制椭圆

	roundrect(-300, 150, 300, -150, 120, 60);//绘制圆角矩形

	setcolor(BLACK);
	pie(-200, 100, 200, -100, 0, PI / 4);

	setcolor(GREEN);
	setfillcolor(GREEN);
	fillcircle(100, 100, 50);

	_getch();//暂停窗口等待用户输入
	closegraph();//关闭窗口
	return 0;
}