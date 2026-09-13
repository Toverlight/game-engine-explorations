#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>
#include <conio.h>
#include <Windows.h>
#define WIN_WIDTH 475
#define WIN_HEIGHT 600
IMAGE backGround;

void gameInit()
{
	loadimage(&backGround, "./AdorePaimon.bmp");
	putimage(-300, -50, &backGround);
}

int main()
{
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	gameInit();
	while (1)
	{
		;
	}
	return 0;
}