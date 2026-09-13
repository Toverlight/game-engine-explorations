#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>
#include <conio.h>
#include <time.h>

#define WIDTH 800  // 窗口宽度
#define HEIGHT 600  // 窗口高度
#define SIZE 20  // 蛇身和食物的大小

enum Direction { UP, DOWN, LEFT, RIGHT };  // 蛇的移动方向

struct Point {
    int x;
    int y;
};

int main()
{
    // 初始化窗口
    initgraph(WIDTH, HEIGHT);
    setbkcolor(BLACK);

    // 初始化蛇和食物
    int length = 3;  // 蛇的初始长度
    Direction dir = RIGHT;  // 蛇的初始移动方向
    Point snake[100];  // 蛇的身体
    snake[0].x = 100; snake[0].y = 100;  // 蛇头的初始位置
    snake[1].x = 80; snake[1].y = 100;  // 蛇身的初始位置
    snake[2].x = 60; snake[2].y = 100;  // 蛇尾的初始位置
    Point food;  // 食物的位置
    srand((unsigned)time(NULL));  // 随机数种子

    // 游戏循环
    while (true) {
        // 绘制蛇头和身体
        setfillcolor(GREEN);
        for (int i = 0; i < length; i++) {
            solidrectangle(snake[i].x, snake[i].y, snake[i].x + SIZE, snake[i].y + SIZE);
        }

        // 绘制食物
        setfillcolor(RED);
        solidrectangle(food.x, food.y, food.x + SIZE, food.y + SIZE);

        // 蛇的移动
        for (int i = length - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
        switch (dir) {
        case UP:
            snake[0].y -= SIZE;
            break;
        case DOWN:
            snake[0].y += SIZE;
            break;
        case LEFT:
            snake[0].x -= SIZE;
            break;
        case RIGHT:
            snake[0].x += SIZE;
            break;
        }

        // 判断是否吃到食物
        if (snake[0].x == food.x && snake[0].y == food.y) {
            length++;  // 蛇的长度增加
            // 随机生成新的食物位置
            food.x = rand() % (WIDTH / SIZE) * SIZE;
            food.y = rand() % (HEIGHT / SIZE) * SIZE;
        }

        // 判断是否撞墙或撞到自己的身体，游戏结束
        if (snake[0].x < 0 || snake[0].x >= WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT)
            break;
    }
    return 0;
}