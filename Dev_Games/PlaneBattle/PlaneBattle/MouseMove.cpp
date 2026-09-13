#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>
int main() {
	initgraph(1280, 720);
	setfillcolor(YELLOW);

	int x = 300;
	int y = 300;

	BeginBatchDraw();

	while (true)
	{
		ExMessage msg;
		while (peekmessage(&msg))
		{
			if (msg.message == WM_MOUSEMOVE)
			{
				x = msg.x;
				y = msg.y;
			}
		}

		cleardevice();
		solidcircle(x, y, 30);
		FlushBatchDraw();
	}
	EndBatchDraw();
	return 0;
}