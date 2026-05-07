#include "Airplay.h"
#include <math.h>

IMAGE img[5];
IMAGE mask[5];

void makeTransparent(IMAGE* src, IMAGE* maskImg)
{
    // 后续用于处理图片透明背景
}

void init()
{
    initgraph(bgWidth, bgHeight);
    setbkmode(TRANSPARENT);
}

// 初始化我方飞机
void initMyPlane(MyPlane* plane)
{
    plane->x = (bgWidth - myairWidth) / 2;
    plane->y = bgHeight - myairHeight - 10;
    plane->speed = 5;
    plane->alive = true;
}

// 控制我方飞机移动
void moveMyPlane(MyPlane* plane)
{
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        if (plane->y > 0)
            plane->y -= plane->speed;
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        if (plane->y < bgHeight - myairHeight)
            plane->y += plane->speed;
    }

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        if (plane->x > 0)
            plane->x -= plane->speed;
    }

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        if (plane->x < bgWidth - myairWidth)
            plane->x += plane->speed;
    }
}

// 绘制我方飞机
void drawMyPlane(MyPlane* plane)
{
    if (plane->alive)
    {
        rectangle(
            plane->x,
            plane->y,
            plane->x + myairWidth,
            plane->y + myairHeight
        );
    }
}

void start()
{
    init();

    MyPlane myPlane;
    initMyPlane(&myPlane);

    while (myPlane.alive)
    {
        cleardevice();

        moveMyPlane(&myPlane);
        drawMyPlane(&myPlane);

        Sleep(20);
    }

    closegraph();
}
