#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>
char board_data[3][3] =
{
	{'-', '-', '-'},
	{'-', '-', '-'},
	{'-', '-', '-'},
};
char current_piece = 'o';//初始化棋子类型
bool CheckWin(char c)//判断玩家是否获胜
{
	if (board_data[0][0] == c && board_data[0][1] == c && board_data[0][2] == c) ||
		(board_data[1][0] == c && board_data[1][1] == c && board_data[1][2] == c) ||
		(board_data[2][0] == c && board_data[2][1] == c && board_data[2][2] == c) ||
		(board_data[0][0] == c && board_data[1][0] == c && board_data[2][0] == c) ||
		(board_data[0][1] == c && board_data[1][1] == c && board_data[2][1] == c) ||
		(board_data[0][2] == c && board_data[1][2] == c && board_data[2][2] == c) ||
		(board_data[0][0] == c && board_data[1][1] == c && board_data[2][2] == c) ||
		(board_data[2][0] == c && board_data[1][1] == c && board_data[0][2] == c))
		{
			return 1;
		}
	return 0;
}
bool CheckDraw()//判断是否平局
{

}
void DrawBoard()//绘制棋盘
{

}
void DrawPiece()//绘制棋子
{

}
void DrawTipText()
{

}
int main() {
	initgraph(600, 600);

	bool running = true;

	ExMessage msg;

	BeginBatchDraw();

	while (running)
	{
		//读取操作
		while (peekmessage(&msg))
		{
			//检查鼠标左键按下消息
			if (msg.message == WM_LBUTTONDOWN)
			{
				//计算点击位置
				int x = msg.x;
				int y = msg.y;

				int index_x = x / 200;
				int index_y = y / 200;
				//落子
				if (board_data[index_y][index_x] == '-')
				{
					//只有当前格子为空时，才能放置棋子
					board_data[index_y][index_x] = current_piece;
					if (current_piece == 'o')
					{
						current_piece = 'x';
					}
					else
					{
						current_piece == 'o';
					}
				}
				//检测胜负状况
				if (CheckWin('x'))
				{
					MessageBox(GetHWnd(), _T("x 玩家获胜"), _T("游戏结束"), MB_OK);
					running = false;
				}
				else if (CheckWin('o'))
				{
					MessageBox(GetHWnd(), _T("o 玩家获胜"), _T("游戏结束"), MB_OK);
					running = false;
				}
				else if (CheckDraw())
				{
					MessageBox(GetHWnd(), _T("平局！"),_T("游戏结束"), MB_OK);
					running = False;
				}

				cleardevice();

				Drawboard();
				DrawPiece();
			}
		}

		//数据处理

		//绘制图像
	}
	return 0;
}