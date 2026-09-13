#include <stdio.h>
#include <easyx.h>
#include <time.h>//clock
#include <Windows.h>//Sleep
#include <mmsystem.h>//mci
#pragma comment(lib,"winmm.lib")//静态链接库
#pragma comment(lib, "msimg32.lib")//AlphaBlend函数所在库
#define SIZE 64					//每个格子的大小
#define X_NUM 16				//横向格子数
#define Y_NUM 12				//纵向格子数
#define MAIN_TEXT_HEIGHT 72		//主界面字体大小
#define BTN_TEXT_HEIGHT 48		//通常按钮字体大小
#define GAME_TEXT_HEIGHT 36		//游戏时字体大小
#define BOMB_NUM 3				//每个角色最多持有的炸弹数
#define BOMB_QUEUE_LIMIT 10		//玩家扔下的炸弹队列存储的最大数量，通常是最多炸弹数的两倍以上
#define MS_PER_S 1000			//每秒1000毫秒
#define MAX_HEALTH_POINT 3		//最大生命值
#define INV_TIME 1				//玩家受伤害的无敌时间/s
#define FLASH_SWITCH_TIME 0.15	//闪烁二态切换间隔/s
#define	DESTROY_PIECE_TIME 0.3	//损毁碎片贴图持续时间/s
#define MAX_BOOMRADIUS 8		//爆炸半径最大值/格
#define UP_BOOMRADIUS_TIME 10	//爆炸半径增长时间间隔/s

ExMessage msg = { 0 },msg1 = { 0 }, msg2 = { 0 };
IMAGE screenImgs[10] = { 0 };
IMAGE imgs[10] = { 0 };
IMAGE playerImgs[10] = { 0 };
IMAGE cursorImgs[20] = { 0 };
IMAGE bombImgs[10] = { 0 };
IMAGE boomImgs[20] = { 0 };
IMAGE dsyImgs[10] = { 0 };
clock_t frameTime = 1000 / 60;//60帧刷新率，frameTime为每帧耗时/ms

//玩家编号
typedef int Playercode;
//火光朝向
typedef enum {
	UNKNOWN = -1,//所有方向
	UP,//上
	RIGHT,//右
	DOWN,//下
	LEFT//左
}Dir;
//地图块燃烧情况
typedef enum {
	FLAT = 0,//没有燃烧
	FIRING//燃烧中
}Fstate;
//地面
typedef enum {
	NOSURFACE = 0,//无表面
	SOD//草地
}Ground;
//障碍
typedef enum {
	BOMBTAKEN = -1,//存在炸弹
	NOBLOCK = 0,//无障碍
	ROCK,//岩石
	BOX//箱子
}Block;
//障碍摧毁类型
typedef enum {
	NOEVENT = 0,DSY_BOX
}Destroy;
//游戏地图块
typedef struct {
	int ground;//地面
	int block;//障碍
	Fstate boomflame;//火光
	int flameHarm;//火光伤害
	Playercode pcode;//火光从属
	Dir flameDir;//火光朝向
	clock_t totalTime;//火光持续总时间/ms
	clock_t startMoment;//开始时刻/ms
	clock_t curMoment;//当前时刻/ms
	Destroy dsy;//摧毁事件的类型
	clock_t dsy_startMot;//摧毁事件开始时刻/ms
	clock_t dsy_curMot;//摧毁事件开始时刻/ms
	Playercode thePlayerHere;//占据此地的玩家编号
}Map;
Map mpbks[X_NUM][Y_NUM] = {};

//炸弹款式
typedef enum {
	NORMAL = 0
}Style;
//炸弹模板
typedef struct {
	clock_t flameTime;	//火光持续时间
	int atk;			//炸弹攻击力
}BombModel;
//具体炸弹模板
BombModel bmodelNormal = { 0.5 * MS_PER_S, 1 };
//炸弹模板库
BombModel bmodels[5] = { bmodelNormal };
/*炸弹类*/
typedef struct {
	Style style;//炸弹的款式
	clock_t time;//炸弹的从放置到爆炸所需时间/ms
	clock_t cur_time;//记录当前时刻/ms
	clock_t set_moment;//炸弹放置的实际时刻/ms
	int i;//炸弹在地图上的横格索引
	int j;//炸弹在地图上的纵格索引
	bool flag;//炸弹是否有效
	clock_t flameTime;//火光持续时间/ms
	int atk;//炸弹攻击力
}Bomb;
/*玩家类*/
typedef struct {
	int x;							//玩家的横坐标
	int y;							//玩家的纵坐标
	int past_x;						//玩家上一个横坐标
	int past_y;						//玩家上一个纵坐标
	int speed;						//玩家的移动速度
	int vx;							//玩家的横向速度
	int vy;							//玩家的纵向速度
	int bqueue_head;				//炸弹队列头指针
	int bqueue_tail;				//炸弹队列尾指针
	int boomradius;					//玩家固有的炸弹爆炸范围/格
	int hp;							//当前生命值
	clock_t InvTime;				//受到伤害后的无敌时间
	clock_t InvStartMot;			//受到伤害时刻
	clock_t InvCurMot;				//存储当前时刻，用于无敌时间计算
	clock_t FlashHeadMot;			//闪烁头时刻
	clock_t FlashTailMot;			//闪烁尾时刻
	Style bomb_style;				//该玩家放置炸弹的样式
	Bomb bombs[BOMB_QUEUE_LIMIT];	//玩家已经扔下的炸弹
	bool flag;						//该玩家的走动键是否按下
	bool bomb_flag;					//玩家是否可以放置炸弹
	bool flash_flag;				//玩家是否闪烁显示
	bool isShowing;					//闪烁态的玩家是否在显示子态
}Player;
Player player1, player2;
/*广播码*/
typedef enum {
	NONE = 0,BOOM,END,RESTART,EXIT
}McodeText;
//炸弹爆炸广播信息
typedef struct {
	int i;//炸弹爆炸的横坐标
	int j;//炸弹爆炸的纵坐标
	int radius;//炸弹爆炸范围（向四周延申几格）
	Playercode pcode;//玩家编号
	clock_t flameTime;//燃烧时间
}BoomMsg;
//最终伤害来源
typedef enum{
	SELF = 0,OPPONENT
}HarmSrc;
//游戏结束广播信息
typedef struct{
	Playercode outpcode;	//被淘汰的玩家编号
	HarmSrc hmsrc;			//最终伤害来源
}EndMsg;

//广播类
typedef struct {
	McodeText mcode;//广播码
	BoomMsg bmsg;	//炸弹爆炸广播信息
	EndMsg emsg;	//游戏结束广播信息
}Radio;
Radio radio;
//游戏计时器
typedef struct {
	clock_t start;	//开始时刻/ms
	clock_t cur;	//当前时刻/ms
	clock_t time;	//游戏进行时间/ms
}Timer;
Timer timer;
int main_w = SIZE * X_NUM * 0.55 / 1;
int main_h = MAIN_TEXT_HEIGHT * 2 / 1;
int main_x = (SIZE * X_NUM - main_w) / 2;
int main_y = 30;
int btn_w = SIZE * X_NUM * 0.15 / 1;
int btn_h = BTN_TEXT_HEIGHT * 1.3 / 1;
int btn_x = (SIZE * X_NUM - btn_w) / 2;
int btn_y = main_y + main_h + 150;
int btn_eliw = 20, btn_elih = 20;
//游戏初始化
void GameInit()
{
	//计时器初始化
	timer.start = clock();
	timer.cur = clock();
	timer.time = timer.start - timer.start;

	//广播初始化
	radio.mcode = NONE;
	//地图初始化
	for (int i = 0; i < X_NUM; i++)
	{
		for (int j = 0; j < Y_NUM; j++)
		{
			mpbks[i][j].ground = SOD;
			mpbks[i][j].block = NOBLOCK;
			mpbks[i][j].boomflame = FLAT;
			mpbks[i][j].flameDir = UNKNOWN;
			mpbks[i][j].totalTime = 0;
			mpbks[i][j].startMoment = 0;
			mpbks[i][j].curMoment = 0;
			mpbks[i][j].flameHarm = 0;
			mpbks[i][j].dsy = NOEVENT;
			mpbks[i][j].thePlayerHere = 0;
		}
	}
	//地图的批量初始化数据
	int groundInitMatrix[Y_NUM][X_NUM] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	int blockInitMatrix[Y_NUM][X_NUM] = {
		{0,0,1,2,0,0,0,0,2,2,2,0,0,2,0,0},
		{0,0,0,2,0,1,1,1,1,1,1,1,0,2,0,2},
		{0,1,0,2,0,0,0,2,2,1,0,0,0,2,0,2},
		{0,1,0,2,1,2,2,2,2,1,2,0,0,2,1,2},
		{0,1,0,2,1,0,2,2,2,1,0,0,0,2,1,2},
		{0,1,0,0,1,0,1,1,2,1,0,0,0,2,1,0},
		{2,1,0,0,1,0,2,2,2,2,0,0,2,0,1,0},
		{2,1,0,0,1,0,1,1,1,1,0,0,2,0,1,2},
		{2,1,0,0,0,2,2,2,2,2,2,0,2,0,1,0},
		{2,1,0,0,0,0,0,1,1,1,0,0,2,0,1,0},
		{2,0,0,0,0,1,1,1,1,0,0,0,2,0,0,0},
		{2,0,1,0,0,0,2,2,2,2,0,0,2,1,0,0},
	};

	//地皮批量初始化&
	//障碍物批量初始化
	for (int j = 0; j < Y_NUM; j++)
	{
		for (int i = 0; i < X_NUM; i++)
		{
			if (groundInitMatrix[j][i] != NOSURFACE)
				mpbks[i][j].ground = groundInitMatrix[j][i];
			
			if (blockInitMatrix[j][i] != NOBLOCK)
				mpbks[i][j].block = blockInitMatrix[j][i];
		}
	}

	//玩家和炸弹初始化
	player1.x = SIZE;
	player1.y = SIZE;
	player1.speed = 5;
	player1.vx = 0;
	player1.vy = 0;
	player1.bomb_style = NORMAL;
	for (int i = 0; i < BOMB_QUEUE_LIMIT; i++)
	{
		player1.bombs[i].style = NORMAL;
		player1.bombs[i].time = 3 * MS_PER_S;
		player1.bombs[i].set_moment = 0;
		player1.bombs[i].cur_time = 0;
		player1.bombs[i].i = -1;
		player1.bombs[i].j = -1;
		player1.bombs[i].flag = false;
		player1.bombs[i].flameTime = bmodels[NORMAL].flameTime;
		player1.bombs[i].atk = bmodels[NORMAL].atk;
	}
	player1.bqueue_head = 0;
	player1.bqueue_tail = 0;
	player1.flag = false;
	player1.boomradius = 1;
	player1.hp = MAX_HEALTH_POINT;
	player1.InvTime = INV_TIME * MS_PER_S;
	player1.InvStartMot = clock();
	player1.InvCurMot = clock();
	player1.flash_flag = false;
	

	player2.x = (X_NUM - 2) * SIZE;
	player2.y = (Y_NUM - 2) * SIZE;
	player2.speed = 5;
	player2.vx = 0;
	player2.vy = 0;
	player2.bomb_style = NORMAL;
	for (int i = 0; i < BOMB_QUEUE_LIMIT; i++)
	{
		player2.bombs[i].style = NORMAL;
		player2.bombs[i].time = 3 * MS_PER_S;
		player2.bombs[i].set_moment = 0;
		player2.bombs[i].cur_time = 0;
		player2.bombs[i].i = -1;
		player2.bombs[i].j = -1;
		player2.bombs[i].flag = false;
		player2.bombs[i].flameTime = bmodels[NORMAL].flameTime;
		player2.bombs[i].atk = bmodels[NORMAL].atk;
	}
	player2.bqueue_head = 0;
	player2.bqueue_tail = 0;
	player2.flag = false;
	player2.boomradius = 1;
	player2.hp = MAX_HEALTH_POINT;
	player2.InvTime = INV_TIME * MS_PER_S;
	player2.InvStartMot = clock();
	player2.InvCurMot = clock();
	player2.flash_flag = false;
}

//加载图片
void LoadImages()
{
	//主界面
	loadimage(&screenImgs[0], "./res/MainScrBk.png", SIZE * X_NUM, SIZE * (Y_NUM + 1));
	loadimage(&screenImgs[1], "./res/EndScreen_mask.png", SIZE * X_NUM, SIZE * (Y_NUM + 1));
	loadimage(&screenImgs[2], "./res/EndScreen.png", SIZE * X_NUM, SIZE * (Y_NUM + 1));

	//地块
	loadimage(&imgs[1], "./res/Ground1.png", SIZE, SIZE);
	loadimage(&imgs[2], "./res/Block1_mask.png", SIZE, SIZE);
	loadimage(&imgs[3], "./res/Block1.png", SIZE, SIZE);
	loadimage(&imgs[4], "./res/Block2_mask.png", SIZE, SIZE);
	loadimage(&imgs[5], "./res/Block2.png", SIZE, SIZE);
	//玩家
	loadimage(&playerImgs[0], "./res/Player1_mask.png", SIZE, SIZE);
	loadimage(&playerImgs[1], "./res/Player1.png", SIZE, SIZE);
	loadimage(&playerImgs[2], "./res/Player2_mask.png", SIZE, SIZE);
	loadimage(&playerImgs[3], "./res/Player2.png", SIZE, SIZE);
	//光标
	loadimage(&cursorImgs[0], "./res/Cursor_up_mask.png", SIZE, SIZE);
	loadimage(&cursorImgs[1], "./res/Cursor_down_mask.png", SIZE, SIZE);
	loadimage(&cursorImgs[2], "./res/Cursor_left_mask.png", SIZE, SIZE);
	loadimage(&cursorImgs[3], "./res/Cursor_right_mask.png", SIZE, SIZE);

	loadimage(&cursorImgs[4], "./res/Cursor_b_up.png", SIZE, SIZE);
	loadimage(&cursorImgs[5], "./res/Cursor_b_down.png", SIZE, SIZE);
	loadimage(&cursorImgs[6], "./res/Cursor_b_left.png", SIZE, SIZE);
	loadimage(&cursorImgs[7], "./res/Cursor_b_right.png", SIZE, SIZE);

	loadimage(&cursorImgs[8], "./res/Cursor_r_up.png", SIZE, SIZE);
	loadimage(&cursorImgs[9], "./res/Cursor_r_down.png", SIZE, SIZE);
	loadimage(&cursorImgs[10], "./res/Cursor_r_left.png", SIZE, SIZE);
	loadimage(&cursorImgs[11], "./res/Cursor_r_right.png", SIZE, SIZE);

	//炸弹
	loadimage(&bombImgs[0], "./res/Bomb1_mask.png", SIZE, SIZE);
	loadimage(&bombImgs[1], "./res/Bomb1_p1.png", SIZE, SIZE);
	loadimage(&bombImgs[2], "./res/Bomb1_p2.png", SIZE, SIZE);
	//火光
	//UNKNOWN
	loadimage(&boomImgs[0], "./res/Boom1_mask.png", SIZE, SIZE);
	loadimage(&boomImgs[1], "./res/Boom1_p1.png", SIZE, SIZE);
	loadimage(&boomImgs[2], "./res/Boom1_p2.png", SIZE, SIZE);
	//UP|DOWN
	loadimage(&boomImgs[3], "./res/Boom2_mask.png", SIZE, SIZE);
	loadimage(&boomImgs[4], "./res/Boom2_p1.png", SIZE, SIZE);
	loadimage(&boomImgs[5], "./res/Boom2_p2.png", SIZE, SIZE);
	//LEFT|RIGHT
	loadimage(&boomImgs[6], "./res/Boom3_mask.png", SIZE, SIZE);
	loadimage(&boomImgs[7], "./res/Boom3_p1.png", SIZE, SIZE);
	loadimage(&boomImgs[8], "./res/Boom3_p2.png", SIZE, SIZE);
	//损毁碎片
	loadimage(&dsyImgs[0], "./res/Destroy_box_mask.png", SIZE, SIZE);
	loadimage(&dsyImgs[1], "./res/Destroy_box.png", SIZE, SIZE);
}
//绘制主界面
//void DrawMainScreen()
//{
//	putimage(0, 0, &imgs[0]);
//	//标题
//	setfillcolor(LIGHTGRAY);
//	fillrectangle(main_x, main_y, main_x + main_w, main_y + main_h);
//	settextcolor(BLUE);
//	settextstyle(MAIN_TEXT_HEIGHT, 0, "思源黑体 Normal");
//	const char* str = "弹   弹   堂";
//	outtextxy(main_x + (main_w - textwidth(str)) / 2, main_y + (main_h - textheight(str)) / 2, str);
//	//按钮
//	setfillcolor(LIGHTGRAY);
//	fillroundrect(btn_x, btn_y, btn_x + btn_w, btn_y + btn_h, btn_eliw, btn_elih);
//
//	const char* btn_t1 = "开始游戏";
//	outtextxy(btn_x + (btn_w - textwidth(btn_t1)) / 2, btn_y + (btn_h - textheight(btn_t1)) / 2, btn_t1);
//}
//判断鼠标是否落在区域内

//加载声音
void LoadAudios()
{
	//音频数组指针的初始化
	const int AUDIONUM = 20;
	const char* audioPaths[AUDIONUM];
	for (int i = 0; i < AUDIONUM; i++)
	{
		audioPaths[i] = NULL;
	}
	//将音频字符串存入字符指针数组中

}

bool inArea(int left,int top,int right,int bottom)
{
	if (msg.x > left && msg.x < right && msg.y > top && msg.y < bottom)
	{
		return true;
	}
	return false;
}
void MainScreenRun()
{
	//clock_t startTime, runTime;
	while (true)
	{
		//startTime = clock();
		cleardevice();
		//背景
		putimage(0, 0, &screenImgs[0]);
		//标题
		setlinecolor(WHITE);
		setfillcolor(LIGHTGRAY);
		fillrectangle(main_x, main_y, main_x + main_w, main_y + main_h);
		settextcolor(BLUE);
		settextstyle(MAIN_TEXT_HEIGHT, 0, "思源黑体 Normal");
		static const char* str = "弹   弹   堂";
		outtextxy(main_x + (main_w - textwidth(str)) / 2, main_y + (main_h - textheight(str)) / 2, str);
		peekmessage(&msg, EX_MOUSE);//获取鼠标消息
		//按钮的触摸模拟
		if (inArea(btn_x, btn_y, btn_x + btn_w, btn_y + btn_h))
		{
			setfillcolor(DARKGRAY);
		}
		else
		{
			setfillcolor(LIGHTGRAY);
		}
		fillroundrect(btn_x, btn_y, btn_x + btn_w, btn_y + btn_h, btn_eliw, btn_elih);
		settextcolor(GREEN);
		settextstyle(BTN_TEXT_HEIGHT, 0, "思源黑体 Heavy");
		static const char* btn_t1 = "开始游戏";
		outtextxy(btn_x + (btn_w - textwidth(btn_t1)) / 2, btn_y + (btn_h - textheight(btn_t1)) / 2, btn_t1);
		//一起绘制
		FlushBatchDraw();
		//检测按钮是否按下
		if (inArea(btn_x, btn_y, btn_x + btn_w, btn_y + btn_h) &&
			msg.message == WM_LBUTTONDOWN)
		{
			//进入游戏
			break;
		}
		//runTime = clock() - startTime;
		//if (frameTime - runTime > 0)
		//{
		//	Sleep(frameTime - runTime);
		//}
	}
}
//绘制地图
void DrawMap()
{
	for (int i = 0; i < X_NUM; i++)
	{
		for (int j = 0; j < Y_NUM; j++)
		{
			//绘制地面
			if (mpbks[i][j].ground == SOD)
				putimage(i * SIZE, j * SIZE, &imgs[1]);

			/*绘制障碍*/
			//绘制岩石
			if (mpbks[i][j].block == ROCK)
			{ 
				putimage(i * SIZE, j * SIZE, &imgs[2], SRCAND);
				putimage(i * SIZE, j * SIZE, &imgs[3], SRCPAINT);
			}
			//绘制箱子
			else if (mpbks[i][j].block == BOX)
			{
				putimage(i * SIZE, j * SIZE, &imgs[4], SRCAND);
				putimage(i * SIZE, j * SIZE, &imgs[5], SRCPAINT);
			}
			/*绘制碎片*/
			if (mpbks[i][j].dsy == DSY_BOX)
			{
				mpbks[i][j].dsy_curMot = clock();
				if (mpbks[i][j].dsy_curMot - mpbks[i][j].dsy_startMot > DESTROY_PIECE_TIME * MS_PER_S)
				{
					mpbks[i][j].dsy = NOEVENT;
				}
				else
				{
					putimage(i * SIZE, j * SIZE, &dsyImgs[0], SRCAND);
					putimage(i * SIZE, j * SIZE, &dsyImgs[1], SRCPAINT);
				}
			}
		}
	}
}
//绘制玩家
void DrawPlayer1()
{
	if (!player1.flash_flag)			//不闪烁显示
	{
		putimage(player1.x, player1.y, &playerImgs[0], SRCAND);
		putimage(player1.x, player1.y, &playerImgs[1], SRCPAINT);
	}
	else								//闪烁显示
	{
		player1.FlashTailMot = clock();
		if (player1.FlashTailMot - player1.FlashHeadMot > FLASH_SWITCH_TIME * MS_PER_S)
		{
			player1.FlashHeadMot = clock();
			player1.isShowing = !player1.isShowing;
		}

		if (player1.isShowing)			//显示子态
		{
			putimage(player1.x, player1.y, &playerImgs[0], SRCAND);
			putimage(player1.x, player1.y, &playerImgs[1], SRCPAINT);
		}
	}

}
void DrawPlayer2()
{
	if (!player2.flash_flag)			//不闪烁显示
	{
		putimage(player2.x, player2.y, &playerImgs[2], SRCAND);
		putimage(player2.x, player2.y, &playerImgs[3], SRCPAINT);
	}
	else								//闪烁显示
	{
		player2.FlashTailMot = clock();
		if (player2.FlashTailMot - player2.FlashHeadMot > FLASH_SWITCH_TIME * MS_PER_S)
		{
			player2.FlashHeadMot = clock();
			player2.isShowing = !player2.isShowing;
		}

		if (player2.isShowing)			//显示子态
		{
			putimage(player2.x, player2.y, &playerImgs[2], SRCAND);
			putimage(player2.x, player2.y, &playerImgs[3], SRCPAINT);
		}
	}

}
//绘制光标
void DrawCursor1()
{
	if (player1.vy < 0)
	{
		putimage(player1.x, player1.y - SIZE / 2, &cursorImgs[0], SRCAND);
		
		putimage(player1.x, player1.y - SIZE / 2, &cursorImgs[4], SRCPAINT);
	}
	if (player1.vy > 0)
	{
		putimage(player1.x, player1.y + SIZE / 2, &cursorImgs[1], SRCAND);
		putimage(player1.x, player1.y + SIZE / 2, &cursorImgs[5], SRCPAINT);
	}
	if (player1.vx < 0)
	{
		putimage(player1.x - SIZE / 2, player1.y, &cursorImgs[2], SRCAND);
		putimage(player1.x - SIZE / 2, player1.y, &cursorImgs[6], SRCPAINT);
	}
	if (player1.vx > 0)
	{
		putimage(player1.x + SIZE / 2, player1.y, &cursorImgs[3], SRCAND);
		putimage(player1.x + SIZE / 2, player1.y, &cursorImgs[7], SRCPAINT);
	}
}
void DrawCursor2()
{
	if (player2.vy < 0)
	{
		putimage(player2.x, player2.y - SIZE / 2, &cursorImgs[0], SRCAND);
		putimage(player2.x, player2.y - SIZE / 2, &cursorImgs[8], SRCPAINT);
	}
	if (player2.vy > 0)
	{
		putimage(player2.x, player2.y + SIZE / 2, &cursorImgs[1], SRCAND);
		putimage(player2.x, player2.y + SIZE / 2, &cursorImgs[9], SRCPAINT);
	}
	if (player2.vx < 0)
	{
		putimage(player2.x - SIZE / 2, player2.y, &cursorImgs[2], SRCAND);
		putimage(player2.x - SIZE / 2, player2.y, &cursorImgs[10], SRCPAINT);
	}
	if (player2.vx > 0)
	{
		putimage(player2.x + SIZE / 2, player2.y, &cursorImgs[3], SRCAND);
		putimage(player2.x + SIZE / 2, player2.y, &cursorImgs[11], SRCPAINT);
	}
}

//角色控制——角色1——WASD&J键炸弹
void Player1Control()
{
	if (GetAsyncKeyState('W'))
	{
		player1.vx = 0;
		player1.vy = -1;
		printf("W down\n");
	}
	if (GetAsyncKeyState('A'))
	{
		player1.vy = 0;
		player1.vx = -1;
		printf("A down\n");
	}
	if (GetAsyncKeyState('S'))
	{
		player1.vx = 0;
		player1.vy = 1;
		printf("S down\n");
	}
	if (GetAsyncKeyState('D'))
	{
		player1.vy = 0;
		player1.vx = 1;
		printf("D down\n");
	}
	if (GetAsyncKeyState('J'))//放置炸弹💣
	{
		if (player1.bomb_flag &&
			mpbks[(player1.x + SIZE / 2) / SIZE][(player1.y + SIZE / 2) / SIZE].block == 0 &&
			(player1.bqueue_tail - player1.bqueue_head >= 0 ?
				player1.bqueue_tail - player1.bqueue_head :
				(BOMB_QUEUE_LIMIT - (player1.bqueue_head - player1.bqueue_tail))) < BOMB_NUM)
		{
			static Style stl;
			stl = player1.bombs[player1.bqueue_tail].style = player1.bomb_style;
			player1.bombs[player1.bqueue_tail].flameTime = bmodels[stl].flameTime;
			player1.bombs[player1.bqueue_tail].atk = bmodels[stl].atk;
			player1.bombs[player1.bqueue_tail].set_moment = clock();
			player1.bombs[player1.bqueue_tail].i = (player1.x + SIZE / 2) / SIZE;
			player1.bombs[player1.bqueue_tail].j = (player1.y + SIZE / 2) / SIZE;
			player1.bombs[player1.bqueue_tail].flag = true;

			player1.bqueue_tail = (player1.bqueue_tail + 1) % BOMB_QUEUE_LIMIT;

			mpbks[(player1.x + SIZE / 2) / SIZE][(player1.y + SIZE / 2) / SIZE].block = BOMBTAKEN;
		}
	}
				
			
}
//角色控制——角色2——UpDownLeftRight&回车键炸弹
void Player2Control()
{
	if (GetAsyncKeyState(VK_UP))
	{
		player2.vx = 0;
		player2.vy = -1;
		printf("UP down\n");
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		player2.vy = 0;
		player2.vx = -1;
		printf("LEFT down\n");
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		player2.vx = 0;
		player2.vy = 1;
		printf("DOWN down\n");
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		player2.vy = 0;
		player2.vx = 1;
		printf("RIGHT down\n");
	}
	if (GetAsyncKeyState(VK_RETURN))//放置炸弹💣
	{
		if (player2.bomb_flag &&
			mpbks[player2.x / SIZE][player2.y / SIZE].block == 0 &&
			(player2.bqueue_tail - player2.bqueue_head >= 0 ?
				player2.bqueue_tail - player2.bqueue_head :
				(BOMB_QUEUE_LIMIT - (player2.bqueue_head - player2.bqueue_tail))) < BOMB_NUM)
		{
			static Style stl;
			stl = player2.bombs[player2.bqueue_tail].style = player2.bomb_style;
			player2.bombs[player2.bqueue_tail].flameTime = bmodels[stl].flameTime;
			player2.bombs[player2.bqueue_tail].atk = bmodels[stl].atk;
			player2.bombs[player2.bqueue_tail].set_moment = clock();
			player2.bombs[player2.bqueue_tail].i = player2.x / SIZE;
			player2.bombs[player2.bqueue_tail].j = player2.y / SIZE;
			player2.bombs[player2.bqueue_tail].flag = true;

			player2.bqueue_tail = (player2.bqueue_tail + 1) % BOMB_QUEUE_LIMIT;

			mpbks[player2.x / SIZE][player2.y / SIZE].block = BOMBTAKEN;
		}
	}
}
//玩家走动
void Player1Walk()
{
	player1.past_x = player1.x;
	player1.past_y = player1.y;
	//printf("player1.past_x = %d\n", player1.past_x);
	//printf("player1.past_y = %d\n", player1.past_y);
	//printf("player1.x = %d\n", player1.x);
	//printf("player1.y = %d\n", player1.y);

	static int past_vx, past_vy;
	static bool walk_flag,next_to_stop_flag;
	static bool exceed_flag;
	static bool x_flag, y_flag;
	player1.flag = (GetAsyncKeyState('W') || GetAsyncKeyState('A') ||
					GetAsyncKeyState('S') || GetAsyncKeyState('D'));
	//玩家停着
	//获取新的速度方向
	if (player1.flag && !walk_flag)//玩家已经按下了一个方向键且玩家停着
	{
		//记录当前速度方向
		past_vx = player1.vx;
		past_vy = player1.vy;
		//printf("past_vx = %d\n", past_vx);
		//printf("past_vy = %d\n", past_vy);
		walk_flag = true;//开始走动
		next_to_stop_flag = false;//不停下一直走
	}
	//玩家在走但按键已经松开
	if (!player1.flag && walk_flag && !next_to_stop_flag)
		next_to_stop_flag = true;
	//玩家走动中，到下一个格子前，勿扰
	if (walk_flag)
	{
		//printf("总：%d\n", player1.x + player1.speed * past_vx > 0 && player1.x + player1.speed * past_vx < SIZE * (X_NUM - 1) &&
		//	player1.y + player1.speed * past_vy > 0 && player1.y + player1.speed * past_vy < SIZE * (Y_NUM - 1));
		//printf("左：%d\n", player1.x + player1.speed * past_vx > 0);
		//printf("右：%d\n", player1.x + player1.speed * past_vx < SIZE * (X_NUM - 1));
		//printf("上：%d\n", player1.y + player1.speed * past_vy > 0);
		//printf("下：%d\n", player1.y + player1.speed * past_vy < SIZE * (Y_NUM - 1));
		//不会越界
		if (player1.x + player1.speed * past_vx >= 0 && player1.x + player1.speed * past_vx <= SIZE * (X_NUM - 1) &&
			player1.y + player1.speed * past_vy >= 0 && player1.y + player1.speed * past_vy <= SIZE * (Y_NUM - 1))
		{
			//没有东西挡道
			//放心走
			if (mpbks[(player1.x + player1.speed * past_vx + (1 + past_vx) / 2 * SIZE) / SIZE][(player1.y + player1.speed * past_vy + (1 + past_vy) / 2 * SIZE) / SIZE].block == NOBLOCK
				&& mpbks[(player1.x + player1.speed * past_vx + (1 + past_vx) / 2 * SIZE) / SIZE][(player1.y + player1.speed * past_vy + (1 + past_vy) / 2 * SIZE) / SIZE].thePlayerHere != 2)
			{
				player1.x += player1.speed * past_vx;
				player1.y += player1.speed * past_vy;
				player1.bomb_flag = false;
			}
			else
			{
				player1.x = ((player1.x + player1.speed * past_vx) / SIZE + (1 - past_vx) / 2) * SIZE;
				player1.y = ((player1.y + player1.speed * past_vy) / SIZE + (1 - past_vy) / 2) * SIZE;
				walk_flag = false;
				player1.bomb_flag = true;
				return;
			}
		}
		//会越界
		//要越界了
		else
		{
			exceed_flag = true;
			x_flag = true;
			y_flag = true;
			//处理玩家坐标并使其停下
			if (player1.x + player1.speed * past_vx < 0)
			{
				player1.x = 0;
				x_flag = false;
			}
			if (player1.x + player1.speed * past_vx > SIZE * (X_NUM - 1))
			{
				player1.x = SIZE * (X_NUM - 1);
				x_flag = false;
			}
			if (player1.y + player1.speed * past_vy < 0)
			{
				player1.y = 0;
				y_flag = false;
			}
			if (player1.y + player1.speed * past_vy > SIZE * (Y_NUM - 1))
			{
				player1.y = SIZE * (Y_NUM - 1);
				y_flag = false;
			}
			walk_flag = false;
			player1.bomb_flag = true;
			return;
		}
		//中间部分整格判断
		//在走动中
		//横向走动中
		if (player1.past_x != player1.x)
		{
			//走过了一个格子
			//等待下次指令才走
			if (player1.past_x / SIZE != (player1.x + player1.speed * past_vx) / SIZE)
			{
				if (next_to_stop_flag)
				{
					player1.x = ((player1.x + player1.speed * past_vx) / SIZE + (1 - past_vx) / 2 ) * SIZE;
					walk_flag = false;
					player1.bomb_flag = true;
				}
			}
		}
		//纵向走动中
		else if (player1.past_y != player1.y)
		{
			//走过了一个格子
			//等待下次指令才走
			if (player1.past_y / SIZE != (player1.y + player1.speed * past_vy) / SIZE)
			{
				if (next_to_stop_flag)
				{
					player1.y = ((player1.y + player1.speed * past_vy) / SIZE + (1 - past_vy) / 2) * SIZE;
					walk_flag = false;
					player1.bomb_flag = true;
				}
			}
		}
	}
}
void Player2Walk()
{
	player2.past_x = player2.x;
	player2.past_y = player2.y;
	//printf("player2.past_x = %d\n", player2.past_x);
	//printf("player2.past_y = %d\n", player2.past_y);
	//printf("player2.x = %d\n", player2.x);
	//printf("player2.y = %d\n", player2.y);

	static int past_vx, past_vy;
	static bool walk_flag,next_to_stop_flag;
	static bool exceed_flag;
	static bool x_flag, y_flag;
	player2.flag = (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_LEFT) ||
		GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(VK_RIGHT));
	//玩家停着
	//获取新的速度方向
	if (player2.flag && !walk_flag)//玩家已经按下了一个方向键且玩家停着
	{
		//记录当前速度方向
		past_vx = player2.vx;
		past_vy = player2.vy;
		//printf("past_vx = %d\n", past_vx);
		//printf("past_vy = %d\n", past_vy);
		walk_flag = true;//开始走动
		next_to_stop_flag = false;//不停下一直走
	}
	//玩家在走但按键已经松开
	if (!player2.flag && walk_flag && !next_to_stop_flag)
		next_to_stop_flag = true;
	//玩家走动中，到下一个格子前，勿扰
	if (walk_flag)
	{
		//printf("总：%d\n", player2.x + player2.speed * past_vx > 0 && player2.x + player2.speed * past_vx < SIZE * (X_NUM - 1) &&
		//	player2.y + player2.speed * past_vy > 0 && player2.y + player2.speed * past_vy < SIZE * (Y_NUM - 1));
		//printf("左：%d\n", player2.x + player2.speed * past_vx > 0);
		//printf("右：%d\n", player2.x + player2.speed * past_vx < SIZE * (X_NUM - 1));
		//printf("上：%d\n", player2.y + player2.speed * past_vy > 0);
		//printf("下：%d\n", player2.y + player2.speed * past_vy < SIZE * (Y_NUM - 1));
		//不会越界
		if (player2.x + player2.speed * past_vx >= 0 && player2.x + player2.speed * past_vx <= SIZE * (X_NUM - 1) &&
			player2.y + player2.speed * past_vy >= 0 && player2.y + player2.speed * past_vy <= SIZE * (Y_NUM - 1))
		{
			//没有东西挡道
			//放心走
			if (mpbks[(player2.x + player2.speed * past_vx + (1 + past_vx) / 2 * SIZE) / SIZE][(player2.y + player2.speed * past_vy + (1 + past_vy) / 2 * SIZE) / SIZE].block == NOBLOCK
				&& mpbks[(player2.x + player2.speed * past_vx + (1 + past_vx) / 2 * SIZE) / SIZE][(player2.y + player2.speed * past_vy + (1 + past_vy) / 2 * SIZE) / SIZE].thePlayerHere != 1)
			{
				player2.x += player2.speed * past_vx;
				player2.y += player2.speed * past_vy;
				player2.bomb_flag = false;
			}
			else
			{
				player2.x = ((player2.x + player2.speed * past_vx) / SIZE + (1 - past_vx) / 2) * SIZE;
				player2.y = ((player2.y + player2.speed * past_vy) / SIZE + (1 - past_vy) / 2) * SIZE;
				walk_flag = false;
				player2.bomb_flag = true;
				return;
			}
		}
		//会越界
		//要越界了
		else
		{
			exceed_flag = true;
			x_flag = true;
			y_flag = true;
			//处理玩家坐标并使其停下
			if (player2.x + player2.speed * past_vx < 0)
			{
				player2.x = 0;
				x_flag = false;
			}
			if (player2.x + player2.speed * past_vx > SIZE * (X_NUM - 1))
			{
				player2.x = SIZE * (X_NUM - 1);
				x_flag = false;
			}
			if (player2.y + player2.speed * past_vy < 0)
			{
				player2.y = 0;
				y_flag = false;
			}
			if (player2.y + player2.speed * past_vy > SIZE * (Y_NUM - 1))
			{
				player2.y = SIZE * (Y_NUM - 1);
				y_flag = false;
			}
			walk_flag = false;
			player2.bomb_flag = true;
			return;
		}
		//中间部分整格判断
		//在走动中
		//横向走动中
		if (player2.past_x != player2.x)
		{
			//走过了一个格子
			//等待下次指令才走
			if (player2.past_x / SIZE != (player2.x + player2.speed * past_vx) / SIZE)
			{
				if (next_to_stop_flag)
				{
					player2.x = ((player2.x + player2.speed * past_vx) / SIZE + (1 - past_vx) / 2 ) * SIZE;
					walk_flag = false;
					player2.bomb_flag = true;
				}
			}
		}
		//纵向走动中
		else if (player2.past_y != player2.y)
		{
			//走过了一个格子
			//等待下次指令才走
			if (player2.past_y / SIZE != (player2.y + player2.speed * past_vy) / SIZE)
			{
				if (next_to_stop_flag)
				{
					player2.y = ((player2.y + player2.speed * past_vy) / SIZE + (1 - past_vy) / 2) * SIZE;
					walk_flag = false;
					player2.bomb_flag = true;
				}
			}
		}
	}
}
//被玩家实体占用的地图块标记
void markPlayerTaken()
{
	for (int i = 0; i < X_NUM; i++)
	{
		for (int j = 0; j < Y_NUM; j++)
		{
			if (mpbks[i][j].thePlayerHere == 0)
			{
				if ((player1.x / SIZE == i || (player1.x + SIZE - 1) / SIZE == i) &&
					(player1.y / SIZE == j || (player1.y + SIZE - 1) / SIZE == j))
				{
					mpbks[i][j].thePlayerHere = 1;
				}
				else if ((player2.x / SIZE == i || (player2.x + SIZE - 1) / SIZE == i) &&
					(player2.y / SIZE == j || (player2.y + SIZE - 1) / SIZE == j))
				{
					mpbks[i][j].thePlayerHere = 2;
				}
			}
			else if (mpbks[i][j].thePlayerHere == 1)
			{
				if (!((player1.x / SIZE == i || (player1.x + SIZE - 1) / SIZE == i) &&
					(player1.y / SIZE == j || (player1.y + SIZE - 1) / SIZE == j)))
				{
					mpbks[i][j].thePlayerHere = 0;
				}
			}
			else if (mpbks[i][j].thePlayerHere == 2)
			{
				if (!((player2.x / SIZE == i || (player2.x + SIZE - 1) / SIZE == i) &&
					(player2.y / SIZE == j || (player2.y + SIZE - 1) / SIZE == j)))
				{
					mpbks[i][j].thePlayerHere = 0;
				}
			}
		}
	}
}

//炸弹绘制
void DrawBombs()
{
	//绘制玩家1的炸弹
	for (int k = player1.bqueue_head;  k != player1.bqueue_tail; k = (k + 1) % BOMB_QUEUE_LIMIT)
	{
		if (player1.bombs[k].flag)
		{
			putimage(player1.bombs[k].i * SIZE, player1.bombs[k].j * SIZE, &bombImgs[0], SRCAND);
			putimage(player1.bombs[k].i * SIZE, player1.bombs[k].j * SIZE, &bombImgs[1], SRCPAINT);
		}
	}
	//绘制玩家2的炸弹
	for (int k = player2.bqueue_head;  k != player2.bqueue_tail; k = (k + 1) % BOMB_QUEUE_LIMIT)
	{
		if (player2.bombs[k].flag)
		{
			putimage(player2.bombs[k].i * SIZE, player2.bombs[k].j * SIZE, &bombImgs[0], SRCAND);
			putimage(player2.bombs[k].i * SIZE, player2.bombs[k].j * SIZE, &bombImgs[2], SRCPAINT);
		}
	}
}

//炸弹控制
void P1BombRun()
{
	//炸弹的有效性控制
	for (int k = player1.bqueue_head; k != player1.bqueue_tail; k = (k + 1) % BOMB_QUEUE_LIMIT)
	{
		//获取当前时刻
		if (player1.bombs[k].flag)
		{
			player1.bombs[k].cur_time = clock();
		}
		//炸弹有效性处理
		if (player1.bombs[k].cur_time - player1.bombs[k].set_moment >= player1.bombs[k].time)
		{
			player1.bombs[k].flag = false;
		}
		//如果满足该if条件就说明这个炸弹开始爆炸
		if (!player1.bombs[k].flag)
		{
			radio.mcode = BOOM;
			radio.bmsg.radius = player1.boomradius;
			radio.bmsg.i = player1.bombs[k].i;
			radio.bmsg.j = player1.bombs[k].j;
			radio.bmsg.pcode = 1;
			radio.bmsg.flameTime = bmodels[player1.bombs[k].style].flameTime;

			printf("p1set to 0,recycled\n");
			mpbks[player1.bombs[k].i][player1.bombs[k].j].block = NOBLOCK;
			player1.bqueue_head = (player1.bqueue_head + 1) % BOMB_QUEUE_LIMIT;
		}
	}
}
void P2BombRun()
{
	//玩家2的
	for (int k = player2.bqueue_head; k != player2.bqueue_tail; k = (k + 1) % BOMB_QUEUE_LIMIT)
	{
		//获取当前时刻
		if (player2.bombs[k].flag)
		{
			player2.bombs[k].cur_time = clock();
		}
		//炸弹有效性处理
		if (player2.bombs[k].cur_time - player2.bombs[k].set_moment >= player2.bombs[k].time)
		{
			player2.bombs[k].flag = false;
		}
		//如果满足该if条件就说明这个炸弹开始爆炸
		if (!player2.bombs[k].flag)
		{
			radio.mcode = BOOM;
			radio.bmsg.radius = player2.boomradius;
			radio.bmsg.i = player2.bombs[k].i;
			radio.bmsg.j = player2.bombs[k].j;
			radio.bmsg.pcode = 2;
			radio.bmsg.flameTime = bmodels[player2.bombs[k].style].flameTime;

			printf("p2set to 0,recycled\n");
			mpbks[player2.bombs[k].i][player2.bombs[k].j].block = NOBLOCK;
			player2.bqueue_head = (player2.bqueue_head + 1) % BOMB_QUEUE_LIMIT;
		}
	}
}
//地块点燃处理
void lightupby(int i = radio.bmsg.i,int j = radio.bmsg.j,Dir flameDir = UNKNOWN)
{
	//空地点燃
	mpbks[i][j].boomflame = FIRING;
	mpbks[i][j].pcode = radio.bmsg.pcode;
	mpbks[i][j].flameDir = flameDir;
	mpbks[i][j].totalTime = radio.bmsg.flameTime;
	mpbks[i][j].startMoment = clock();
	if (radio.bmsg.pcode == 1)
	{
		mpbks[i][j].flameHarm = bmodels[player1.bomb_style].atk;
	}
	else if (radio.bmsg.pcode == 2)
	{
		mpbks[i][j].flameHarm = bmodels[player2.bomb_style].atk;
	}
	//可炸破的还需处理:
	if (mpbks[i][j].block == BOX)
	{
		//箱子破坏
		mpbks[i][j].dsy = DSY_BOX;
		mpbks[i][j].dsy_startMot = clock();
		//障碍清除
		mpbks[i][j].block = NOBLOCK;
	}
}
//判断当前地块能否被点燃
bool isflammable(int i, int j)
{
	if (mpbks[i][j].block == NOBLOCK || mpbks[i][j].block == BOX)
	{
		return true;
	}
	return false;
}
//地块熄灭检查与处理
void checkAndExtinguish()
{
	static clock_t curMt,startMt,totalT;
	for (int i = 0; i < X_NUM; i++)
	{
		for (int j = 0; j < Y_NUM; j++)
		{
			//燃烧的，判断现在是否应该熄灭
			if (mpbks[i][j].boomflame == FIRING)
			{
				curMt = mpbks[i][j].curMoment = clock();
				startMt = mpbks[i][j].startMoment;
				totalT = mpbks[i][j].totalTime;
				//持续时间到
				if (curMt - startMt >= totalT)
				{
					mpbks[i][j].boomflame = FLAT;
					mpbks[i][j].flameHarm = 0;
				}
			}
		}
	}
}

/*炸弹火光逻辑*/
void Boomflame()
{
	//四个方向的flag
	static bool dirFlags[4];
	for (int i = 0; i < 4; i++)
	{
		dirFlags[i] = true;
	}
	//如果有炸弹爆炸广播信息
	if (radio.mcode == BOOM)
	{
		/*处理火光的亮*/
		//爆炸中心
		lightupby();
		//四周
		for (int d = 1; d <= radio.bmsg.radius; d++)
		{
			//UP
			if (radio.bmsg.j - d >= 0 &&
				isflammable(radio.bmsg.i, radio.bmsg.j - d) &&
				dirFlags[UP])
			{
				lightupby(radio.bmsg.i, radio.bmsg.j - d, UP);
			}
			else
			{
				dirFlags[UP] = false;
			}
			//RIGHT
			if (radio.bmsg.i + d < X_NUM &&
				isflammable(radio.bmsg.i + d, radio.bmsg.j) &&
				dirFlags[RIGHT])
			{
				lightupby(radio.bmsg.i + d, radio.bmsg.j, RIGHT);
			}
			else
			{
				dirFlags[RIGHT] = false;
			}
			//DOWN
			if (radio.bmsg.j + d < Y_NUM &&
				isflammable(radio.bmsg.i, radio.bmsg.j + d) &&
				dirFlags[DOWN])
			{
				lightupby(radio.bmsg.i, radio.bmsg.j + d, DOWN);
			}
			else
			{
				dirFlags[DOWN] = false;
			}
			//LEFT
			if (radio.bmsg.i - d >= 0 &&
				isflammable(radio.bmsg.i - d, radio.bmsg.j) &&
				dirFlags[LEFT])
			{
				lightupby(radio.bmsg.i - d, radio.bmsg.j, LEFT);
			}
			else
			{
				dirFlags[LEFT] = false;
			}
		}
	}
	//处理火光的灭
	checkAndExtinguish();
}

//绘制炸弹火光
void Drawboomflame()
{
	//遍历地图，绘制炸弹火光
	for (int i = 0; i < X_NUM; i++)
	{
		for (int j = 0; j < Y_NUM; j++)
		{
			//如果检查到燃烧块，则绘制火光
			if (mpbks[i][j].boomflame == FIRING)
			{
				if (mpbks[i][j].pcode == 1)
				{
					switch (mpbks[i][j].flameDir)
					{
					case UNKNOWN:
						putimage(i * SIZE, j * SIZE, &boomImgs[0], SRCAND);
						putimage(i * SIZE, j * SIZE, &boomImgs[1], SRCPAINT);
						break;
					case UP:case DOWN:
						putimage(i * SIZE, j * SIZE, &boomImgs[3], SRCAND);
						putimage(i * SIZE, j * SIZE, &boomImgs[4], SRCPAINT);
						break;
					case LEFT:case RIGHT:
						putimage(i * SIZE, j * SIZE, &boomImgs[6], SRCAND);
						putimage(i * SIZE, j * SIZE, &boomImgs[7], SRCPAINT);
						break;
					}
				}				
				else if (mpbks[i][j].pcode == 2)
				{
					switch (mpbks[i][j].flameDir)
					{
					case UNKNOWN:
						putimage(i * SIZE, j * SIZE, &boomImgs[0], SRCAND);
						putimage(i * SIZE, j * SIZE, &boomImgs[2], SRCPAINT);
						break;
					case UP:case DOWN:
						putimage(i * SIZE, j * SIZE, &boomImgs[3], SRCAND);
						putimage(i * SIZE, j * SIZE, &boomImgs[5], SRCPAINT);
						break;
					case LEFT:case RIGHT:
						putimage(i * SIZE, j * SIZE, &boomImgs[6], SRCAND);
						putimage(i * SIZE, j * SIZE, &boomImgs[8], SRCPAINT);
						break;
					}
				}
			}
		}
	}
}
//绘制游戏信息
void DrawInfo()
{
	/*从(0, Y_NUM * SIZE)画到(getwidth(), getheight())*/
	//背景填充
	setlinestyle(PS_DOT, 5);
	setlinecolor(RGB(230, 231, 239));
	setfillcolor(RGB(195, 195, 195));
	fillrectangle(0, Y_NUM * SIZE, getwidth(), getheight());
	//信息块块体
	static int w_hp = getwidth() / 5;
	static int w_br = getwidth() / 6;
	static int h = SIZE * 0.8;
	static int x1_hp = getwidth() / 15 * 2 - w_hp / 2;
	static int x1_br = getwidth() / 8 * 3 - w_br / 2;
	static int x2_hp = getwidth() / 15 * 13 - w_hp / 2;
	static int x2_br = getwidth() / 8 * 5 - w_br / 2;
	static int y = getheight() - SIZE / 2 - h / 2;
	setlinestyle(PS_SOLID, 4);
	setlinecolor(RGB(0, 92, 185));
	setfillcolor(RGB(255, 237, 200));
	fillrectangle(x1_hp, y, x1_hp + w_hp, y + h);
	fillrectangle(x1_br, y, x1_br + w_br, y + h);
	setlinecolor(RGB(252, 61, 73));
	fillrectangle(x2_hp, y, x2_hp + w_hp, y + h);
	fillrectangle(x2_br, y, x2_br + w_br, y + h);

	setlinecolor(GREEN);
	setfillcolor(RGB(244, 114, 122));
	fillcircle(getwidth() / 2, getheight() - SIZE / 2, SIZE / 2 - 5);
	/*信息文本*/
	//剩余弹药显示
	settextcolor(BLUE);
	settextstyle(GAME_TEXT_HEIGHT, 0, "思源黑体 Normal");
	static char brstr[30];
	sprintf_s(brstr, "剩余炸弹：%d", (player1.bqueue_tail - player1.bqueue_head >= 0?
		BOMB_NUM - (player1.bqueue_tail - player1.bqueue_head) : 
		BOMB_NUM - (BOMB_QUEUE_LIMIT - (player1.bqueue_head - player1.bqueue_tail))));
	outtextxy(x1_br + (w_br - textwidth(brstr)) / 2, y + (h - textheight(brstr)) / 2, brstr);

	settextcolor(RED);
	settextstyle(GAME_TEXT_HEIGHT, 0, "思源黑体 Normal");
	static char brstr2[30];
	sprintf_s(brstr2, "剩余炸弹：%d", (player2.bqueue_tail - player2.bqueue_head >= 0?
		BOMB_NUM - (player2.bqueue_tail - player2.bqueue_head) : 
		BOMB_NUM - (BOMB_QUEUE_LIMIT - (player2.bqueue_head - player2.bqueue_tail))));
	outtextxy(x2_br + (w_br - textwidth(brstr2)) / 2, y + (h - textheight(brstr2)) / 2, brstr2);
	//生命显示
	settextcolor(BLUE);
	settextstyle(GAME_TEXT_HEIGHT, 0, "思源黑体 Heavy");
	static char hpstr[30];
	sprintf_s(hpstr, "当前生命：%d", player1.hp);
	outtextxy(x1_hp + (w_hp - textwidth(hpstr)) / 2, y + (h - textheight(hpstr)) / 2, hpstr);

	settextcolor(RED);
	settextstyle(GAME_TEXT_HEIGHT, 0, "思源黑体 Heavy");
	static char hpstr2[30];
	sprintf_s(hpstr2, "当前生命：%d", player2.hp);
	outtextxy(x2_hp + (w_hp - textwidth(hpstr2)) / 2, y + (h - textheight(hpstr2)) / 2, hpstr2);
	/*信息条*/
	//生命条显示
	setlinecolor(RGB(0, 92, 185));
	rectangle(5, y, x1_hp - 5, y + h);
	setfillcolor(BLUE);
	solidrectangle(7, y + 2 + (h - 4) * (1.0 - (player1.hp * 1.0 / MAX_HEALTH_POINT)), x1_hp - 7, y + h - 3);

	setlinecolor(RGB(252, 61, 73));
	rectangle(getwidth() - 5, y, x2_hp + w_hp + 5, y + h);
	setfillcolor(RED);
	solidrectangle(getwidth() - 7, y + 2 + (h - 4) * (1.0 - (player2.hp * 1.0 / MAX_HEALTH_POINT)), x2_hp + w_hp + 7, y + h - 3);
}
//生命值上下限校正
void HP_adjust()
{
	//上限
	if (player1.hp > MAX_HEALTH_POINT)
		player1.hp = MAX_HEALTH_POINT;
	//下限
	if (player1.hp < 0)
		player1.hp = 0;

	//上限
	if (player2.hp > MAX_HEALTH_POINT)
		player2.hp = MAX_HEALTH_POINT;
	//下限
	if (player2.hp < 0)
		player2.hp = 0;
}
//----------------------------------------------
//玩家生命值处理——加血
void AddHP()
{

	HP_adjust();
}
//---------------------------------------------
//角色死亡发送结束广播
void EndBroadcast(HarmSrc& hmsrc1,HarmSrc& hmsrc2)
{
	//当前任意一位玩家生命值归零，则发送游戏结束广播
	if (player1.hp == 0)
	{
		radio.mcode = END;
		radio.emsg.outpcode = 1;
		radio.emsg.hmsrc = hmsrc1;
	}
	else if (player2.hp == 0)
	{
		radio.mcode = END;
		radio.emsg.outpcode = 2;
		radio.emsg.hmsrc = hmsrc2;
	}
	
}
/*索引坐标类*/
typedef struct {
	int i;//横索引
	int j;//纵索引
}Pos;

/*四角类*/
typedef enum {
	ZERO = 0,LEFT_UP,RIGHT_UP,LEFT_DOWN,RIGHT_DOWN
}Edge;

//判断是哪个角判定伤害，如无伤害，返回0
Edge JudgeEdges(Pos& lp,Pos& ln,Pos& rp, Pos& rn)
{
	if (mpbks[lp.i][lp.j].boomflame == FIRING)
		return LEFT_UP;
	if (mpbks[ln.i][ln.j].boomflame == FIRING)
		return LEFT_DOWN;
	if (mpbks[rp.i][rp.j].boomflame == FIRING)
		return RIGHT_UP;
	if (mpbks[rn.i][rn.j].boomflame == FIRING)
		return RIGHT_DOWN;
	return ZERO;
}

//玩家生命值处理——扣血
void SubHP()
{
	//建立存储两位玩家伤害来源信息的两个变量
	static HarmSrc hmsrc1,hmsrc2;
	//建立存储玩家四个角坐标信息的两个变量
	static Pos leftup1, leftup2;
	static Pos rightup1, rightup2;
	static Pos leftdown1, leftdown2;
	static Pos rightdown1, rightdown2;
	const float T = 0.7f;
	Edge edge1, edge2;
	//---------------------------------------------------------
	//判断这位玩家是否在火光区
	//需要考虑碰撞箱（不妨规定玩家贴图正方形的0.7倍中央子正方形为判定有效区）
	//方法是只需判断四个角是否落在火光格中，左闭右开
	//首先，更新玩家碰撞箱四角坐标信息
	leftup1 = { (int)(player1.x + SIZE * (1 - T) / 2) / SIZE,(int)(player1.y + SIZE * (1 - T) / 2) / SIZE };
	rightup1 = { (int)(player1.x + SIZE - SIZE * (1 - T) / 2) / SIZE,(int)(player1.y + SIZE * (1 - T) / 2) / SIZE };
	leftdown1 = { (int)(player1.x + SIZE * (1 - T) / 2) / SIZE, (int)(player1.y + SIZE - SIZE * (1 - T) / 2) / SIZE };
	rightdown1 = { (int)(player1.x + SIZE - SIZE * (1 - T) / 2) / SIZE, (int)(player1.y + SIZE - SIZE * (1 - T) / 2) / SIZE };
	leftup2 = { (int)(player2.x + SIZE * (1 - T) / 2) / SIZE,(int)(player2.y + SIZE * (1 - T) / 2) / SIZE };
	rightup2 = { (int)(player2.x + SIZE - SIZE * (1 - T) / 2) / SIZE,(int)(player2.y + SIZE * (1 - T) / 2) / SIZE };
	leftdown2 = { (int)(player2.x + SIZE * (1 - T) / 2) / SIZE, (int)(player2.y + SIZE - SIZE * (1 - T) / 2) / SIZE };
	rightdown2 = { (int)(player2.x + SIZE - SIZE * (1 - T) / 2) / SIZE, (int)(player2.y + SIZE - SIZE * (1 - T) / 2) / SIZE };
	//如果玩家踩到火光格上且无敌时间已过，则扣血并刷新无敌时间
	player1.InvCurMot = clock();
	if (player1.InvCurMot - player1.InvStartMot > player1.InvTime)
	{
		if (player1.flash_flag)player1.flash_flag = false;//超过无敌时间，玩家就不闪了
		if (edge1 = JudgeEdges(leftup1,leftdown1,rightup1,rightdown1))
		{
			player1.flash_flag = true;//不在无敌时间且受伤，闪烁显示
			player1.FlashHeadMot = clock();
			player1.isShowing = true;//重置玩家显示子态
			switch (edge1)
			{
			case LEFT_UP:
				player1.hp -= mpbks[leftup1.i][leftup1.j].flameHarm;	//扣血
				player1.InvStartMot = clock();							//刷新无敌时间
				if (mpbks[leftup1.i][leftup1.j].pcode == 1)				//自己对自己造成伤害
					hmsrc1 = HarmSrc::SELF;
				else if (mpbks[leftup1.i][leftup1.j].pcode == 2)		//敌人对自己造成伤害
					hmsrc1 = HarmSrc::OPPONENT;
				break;
			case RIGHT_UP:
				player1.hp -= mpbks[rightup1.i][rightup1.j].flameHarm;	//扣血
				player1.InvStartMot = clock();							//刷新无敌时间
				if (mpbks[rightup1.i][rightup1.j].pcode == 1)			//自己对自己造成伤害
					hmsrc1 = HarmSrc::SELF;
				else if (mpbks[rightup1.i][rightup1.j].pcode == 2)		//敌人对自己造成伤害
					hmsrc1 = HarmSrc::OPPONENT;
				break;
			case LEFT_DOWN:
				player1.hp -= mpbks[leftdown1.i][leftdown1.j].flameHarm;//扣血
				player1.InvStartMot = clock();							//刷新无敌时间
				if (mpbks[leftdown1.i][leftdown1.j].pcode == 1)			//自己对自己造成伤害
					hmsrc1 = HarmSrc::SELF;
				else if (mpbks[leftdown1.i][leftdown1.j].pcode == 2)	//敌人对自己造成伤害
					hmsrc1 = HarmSrc::OPPONENT;
				break;
			case RIGHT_DOWN:
				player1.hp -= mpbks[rightdown1.i][rightdown1.j].flameHarm;	//扣血
				player1.InvStartMot = clock();								//刷新无敌时间
				if (mpbks[rightdown1.i][rightdown1.j].pcode == 1)			//自己对自己造成伤害
					hmsrc1 = HarmSrc::SELF;
				else if (mpbks[rightdown1.i][rightdown1.j].pcode == 2)		//敌人对自己造成伤害
					hmsrc1 = HarmSrc::OPPONENT;
				break;
			}
		}
	}
	player2.InvCurMot = clock();
	if (player2.InvCurMot - player2.InvStartMot > player2.InvTime)
	{
		if (player2.flash_flag)player2.flash_flag = false;//超过无敌时间，玩家就不闪了
		if (edge2 = JudgeEdges(leftup2,leftdown2,rightup2,rightdown2))
		{
			player2.flash_flag = true;//不在无敌时间且受伤，闪烁显示
			player2.FlashHeadMot = clock();
			player2.isShowing = true;//重置玩家显示子态
			switch (edge2)
			{
			case LEFT_UP:
				player2.hp -= mpbks[leftup2.i][leftup2.j].flameHarm;	//扣血
				player2.InvStartMot = clock();							//刷新无敌时间
				if (mpbks[leftup2.i][leftup2.j].pcode == 2)				//自己对自己造成伤害
					hmsrc2 = HarmSrc::SELF;
				else if (mpbks[leftup2.i][leftup2.j].pcode == 1)		//敌人对自己造成伤害
					hmsrc2 = HarmSrc::OPPONENT;
				break;
			case RIGHT_UP:
				player2.hp -= mpbks[rightup2.i][rightup2.j].flameHarm;	//扣血
				player2.InvStartMot = clock();							//刷新无敌时间
				if (mpbks[rightup2.i][rightup2.j].pcode == 2)			//自己对自己造成伤害
					hmsrc2 = HarmSrc::SELF;
				else if (mpbks[rightup2.i][rightup2.j].pcode == 1)		//敌人对自己造成伤害
					hmsrc2 = HarmSrc::OPPONENT;
				break;
			case LEFT_DOWN:
				player2.hp -= mpbks[leftdown2.i][leftdown2.j].flameHarm;//扣血
				player2.InvStartMot = clock();							//刷新无敌时间
				if (mpbks[leftdown2.i][leftdown2.j].pcode == 2)			//自己对自己造成伤害
					hmsrc2 = HarmSrc::SELF;
				else if (mpbks[leftdown2.i][leftdown2.j].pcode == 1)	//敌人对自己造成伤害
					hmsrc2 = HarmSrc::OPPONENT;
				break;
			case RIGHT_DOWN:
				player2.hp -= mpbks[rightdown2.i][rightdown2.j].flameHarm;	//扣血
				player2.InvStartMot = clock();								//刷新无敌时间
				if (mpbks[rightdown2.i][rightdown2.j].pcode == 2)			//自己对自己造成伤害
					hmsrc2 = HarmSrc::SELF;
				else if (mpbks[rightdown2.i][rightdown2.j].pcode == 1)		//敌人对自己造成伤害
					hmsrc2 = HarmSrc::OPPONENT;
				break;
			}
		}
	}
	HP_adjust();
	//----------------------------------------------------------
	//角色如果死亡，发送对应结束广播
	EndBroadcast(hmsrc1,hmsrc2);
}
//------------------------------------------------
//游戏结束的内循环判断
bool GameRunningJudge()
{
	//检测到游戏结束广播信号则结束游戏，
	if (radio.mcode == END)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------
//显示与运行结算界面
void EndScreenRun()
{
	while (true)
	{
		cleardevice();
		DrawMap();
		Drawboomflame();
		DrawBombs();
		DrawPlayer1();
		DrawPlayer2();
		DrawInfo();
		//绘制背景
		putimage(0, 0, &screenImgs[1], SRCAND);
		putimage(0, 0, &screenImgs[2], SRCPAINT);
		//绘制文字
		//显示结束广播内容
		settextcolor(BLACK);
		settextstyle(MAIN_TEXT_HEIGHT, 0, "思源黑体 Normal");
		const char* gameover = "游戏结束";
		outtextxy((getwidth() - textwidth(gameover)) / 2, SIZE / 4, gameover);

		settextcolor(BLACK);
		settextstyle(GAME_TEXT_HEIGHT, 0, "思源黑体 Normal");
		static char infoshow[50];
		if (radio.emsg.hmsrc == SELF)
			sprintf_s(infoshow, "玩家 %d 被自己炸死了", radio.emsg.outpcode);
		else if (radio.emsg.hmsrc == OPPONENT)
			sprintf_s(infoshow, "玩家 %d 被玩家 %d 炸死了", radio.emsg.outpcode,
				(radio.emsg.outpcode == 1 ? 2 : 1));
		outtextxy((getwidth() - textwidth(infoshow)) / 2, SIZE / 2 + MAIN_TEXT_HEIGHT, infoshow);		
		
		settextstyle(MAIN_TEXT_HEIGHT, 0, "思源黑体 Heavy");
		static char winnershow[50];
		if (radio.emsg.outpcode == 1)settextcolor(RED);
		else settextcolor(BLUE);
		sprintf_s(winnershow, "玩家 %d 获胜!", (radio.emsg.outpcode == 1 ? 2 : 1));
		outtextxy((getwidth() - textwidth(winnershow)) / 2, SIZE + MAIN_TEXT_HEIGHT + GAME_TEXT_HEIGHT, winnershow);
		//----------------------------------------
		msg.message = 0;
		//绘制按钮
		//按钮大小参数
		static int leftbtn_x = getwidth() / 2 - btn_w - SIZE / 2, rightbtn_x = getwidth() / 2 + SIZE / 2;
		static int leftbtn_y = (getheight() - btn_h) / 2, rightbtn_y = (getheight() - btn_h) / 2;
		//按钮的触摸
		//再来一局按钮
		//若点击“再来一局”，则发送重开广播信息
		peekmessage(&msg, EX_MOUSE);
		if (inArea(leftbtn_x, leftbtn_y,
			leftbtn_x + btn_w, leftbtn_y + btn_h))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				radio.mcode = RESTART;
				break;
			}
			setfillcolor(DARKGRAY);
		}
		else
		{
			setfillcolor(LIGHTGRAY);
		}
		setlinecolor(WHITE);
		setlinestyle(PS_SOLID, 1);
		fillroundrect(leftbtn_x, leftbtn_y, leftbtn_x + btn_w, leftbtn_y + btn_h,
			btn_eliw, btn_elih);
		settextcolor(GREEN);
		settextstyle(BTN_TEXT_HEIGHT, 0, "思源黑体 Heavy");
		static const char* leftbtn_t1 = "再来一局";
		outtextxy(leftbtn_x + (btn_w - textwidth(leftbtn_t1)) / 2, leftbtn_y + (btn_h - textheight(leftbtn_t1)) / 2, leftbtn_t1);
		//退出游戏按钮
		//而点击“结束游戏”，则发送退出广播信息
		peekmessage(&msg, EX_MOUSE);
		if (inArea(rightbtn_x, rightbtn_y,
			rightbtn_x + btn_w, rightbtn_y + btn_h))
		{
			peekmessage(&msg, EX_MOUSE);
			if (msg.message == WM_LBUTTONDOWN)
			{
				radio.mcode = EXIT;
				break;
			}
			setfillcolor(DARKGRAY);
		}
		else
		{
			setfillcolor(LIGHTGRAY);
		}
		fillroundrect(rightbtn_x, rightbtn_y, rightbtn_x + btn_w, rightbtn_y + btn_h,
			btn_eliw, btn_elih);
		settextcolor(GREEN);
		settextstyle(BTN_TEXT_HEIGHT, 0, "思源黑体 Heavy");
		static const char* rightbtn_t1 = "退出游戏";
		outtextxy(rightbtn_x + (btn_w - textwidth(rightbtn_t1)) / 2, rightbtn_y + (btn_h - textheight(rightbtn_t1)) / 2, rightbtn_t1);



		FlushBatchDraw();
	}
}
//外循环重开判断函数决定是否开始新一轮的游戏
bool RestartJudge()
{
	if (radio.mcode == RESTART)
		return true;
	if (radio.mcode == EXIT)
		return false;
	return false;
}
//-------------------------------------------------------------------------------------
//爆炸半径的增长
void UP_BoomRadius() 
{
	timer.cur = clock();
	timer.time = timer.cur - timer.start;
	static int t;
	if (timer.time){}
}

//-------------------------------------------------------------------------------------
int main()
{
	//窗口初始化
	HWND hwnd = initgraph(SIZE * X_NUM, SIZE * (Y_NUM + 1), EX_SHOWCONSOLE);
	setbkcolor(DARKGRAY);
	setbkmode(TRANSPARENT);
	cleardevice();

	LoadImages();
	static clock_t startTime,runTime;
	//DrawMainScreen();
	BeginBatchDraw();
	/*getchar();*/
	//游戏外循环
	while (true)
	{
		MainScreenRun();
		GameInit();
		msg.message = 0;
		msg1.message = 0;
		msg2.message = 0;
		//游戏进行时主循环
		while (true)
		{
			startTime = clock();
			//------------------------------------
			//绘制部分
			//后者覆盖前者，即越往后显示优先级越高
			cleardevice();
			DrawMap();
			Drawboomflame();
			DrawBombs();
			DrawPlayer1();
			DrawPlayer2();
			DrawCursor1();
			DrawCursor2();
			DrawInfo();
			FlushBatchDraw();
			//------------------------------------
			if (!GameRunningJudge())//游戏结束判断
				break;					
			//------------------------------------
			//控制部分
			msg.message = 0;
			msg1.message = 0;
			msg2.message = 0;
			Player1Control();
			Player2Control();
			//------------------------------------
			//逻辑处理部分
			radio.mcode = NONE;	//清零广播码--------

			Player1Walk();
			Player2Walk();

			markPlayerTaken();

			P1BombRun();
			P2BombRun();
			Boomflame();

			AddHP();
			SubHP();
			//------------------------------------

			runTime = clock() - startTime;
			if (frameTime - runTime > 0)
			{
				Sleep(frameTime - runTime);
			}
		}
		EndScreenRun();
		if (!RestartJudge())
			break;
	}
	EndBatchDraw();

	closegraph();
	return 0;
}