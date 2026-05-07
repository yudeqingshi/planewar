#include "Airplay.h"
#include <math.h>

IMAGE img[5];
IMAGE mask[5];

void makeTransparent(IMAGE* src, IMAGE* maskImg)
{
    // 后续版本用于处理图片透明背景
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

// 发射子弹
void fireBullet(MyPlane* plane, LL* bulletList, int* lastFireTime)
{
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        int currentTime = GetTickCount();

        if (currentTime - *lastFireTime > 200)
        {
            int bulletX = plane->x + myairWidth / 2 - bulletWidth / 2;
            int bulletY = plane->y - bulletHeight;

            LinkList_insert(bulletList, bulletX, bulletY, 8);

            *lastFireTime = currentTime;
        }
    }
}

// 绘制子弹
void drawBullet(LL* bulletList)
{
    if (bulletList == NULL || bulletList->head == NULL)
        return;

    Node* temp = bulletList->head;

    while (temp != NULL)
    {
        circle(temp->x, temp->y, bulletWidth / 2);
        temp = temp->next;
    }
}

// 更新子弹位置，并删除飞出屏幕的子弹
void updateBullet(LL* bulletList)
{
    if (bulletList == NULL || bulletList->head == NULL)
        return;

    Node* temp = bulletList->head;
    Node* next;

    while (temp != NULL)
    {
        next = temp->next;

        temp->y -= temp->speed;

        if (temp->y < -bulletHeight)
        {
            LinkList_delete(bulletList, temp);
        }

        temp = next;
    }
}

void start()
{
    init();

    MyPlane myPlane;
    initMyPlane(&myPlane);

    LL* bulletList = LinkList_init();
    int lastFireTime = 0;

    while (myPlane.alive)
    {
        cleardevice();

        moveMyPlane(&myPlane);
        fireBullet(&myPlane, bulletList, &lastFireTime);

        updateBullet(bulletList);

        drawMyPlane(&myPlane);
        drawBullet(bulletList);

        Sleep(20);
    }

    LinkList_destroy(bulletList);
    closegraph();
}
