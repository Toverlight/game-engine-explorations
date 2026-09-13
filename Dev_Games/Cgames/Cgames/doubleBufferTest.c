#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define HEIGHT (20)
#define WIDTH (40)

typedef struct {
	int m_x;
	int m_y;
	int m_char;
}Ball;


typedef struct {
	char m_buffer[WIDTH * HEIGHT];
}Viewport;

//将缓冲区的内容打印到屏幕上
void ViewportToScreen(Viewport* viewport)
{
	for (int j = 0; j < HEIGHT; i++)
	for (int i = 0; i < WIDTH; j++)
	{
		int index = i+j*WIDTH;
		char ch = viewport->m_buffer[index];
		//如果有数据
		if (ch)
		{
			gotoxy(i,j);
			printf("%c", ch);
		}
	}
}

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

//渲染球
void RenderBall(Viewport* viewport,Ball* ball)
{
	int index = ball->m_x * WIDTH + ball->m_y;
	//渲染球的函数需要找到x,y对应的buffer中的index然后更改它
	viewport->m_buffer[index] = ball->m_char;
}



////绘制左球
//void DrawLeft(Ball* b1)
//{
//	gotoxy(b1->m_x, b1->m_y);
//	printf("%c", b1->m_char);
//}
//
////绘制右球
//void DrawRight(Ball* b2)
//{
//	gotoxy(b2->m_x, b2->m_y);
//	printf("%c", b2->m_char);
//}



//等待一秒钟
void Delay()
{
	Sleep(500);
}

//清屏
void clearDevice(Viewport* viewport)
{
	memset(viewport, 0, sizeof(*viewport));
}

int main()
{
	Viewport buffer = { 0 };

	Ball b1, b2;
	b1.m_x = 5;
	b1.m_y = 5;
	b1.m_char = 'b';

	b2.m_x = 3;
	b2.m_y = 7;
	b2.m_char = 'c';

	HideCursor();
	int frame = 0;
	//game loop
	while (1) {
		clearDevice(&buffer);

		//DrawLeft(&b1);
		//Delay();

		//DrawRight(&b2);
		//Delay();

		RenderBall(&buffer,&b1);
		RenderBall(&buffer,&b2);

		gotoxy(0, 0);
		printf("%d", frame);
		Delay();

		frame++;
	}
	return 0;
}