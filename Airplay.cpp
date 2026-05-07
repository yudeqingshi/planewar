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

void initMyPlane(MyPlane* plane)
{
    plane->x = (bgWidth - myairWidth) / 2;
    plane->y = bgHeight - myairHeight - 10;
    plane->speed = 5;
    plane->alive = true;
}

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

// 生成敌机
void generateEnemy(LL* enemyList, int spawnRate, int minSpeed, int maxSpeed, bool spawnBigEnemy)
{
    if (rand() % spawnRate == 0)
    {
        if (spawnBigEnemy && rand() % 5 == 0)
        {
            int x = rand() % (bgWidth - bigEnemyWidth);
            int speed = minSpeed / 2;

            Node* newEnemy = Node_init(x, 0, speed);
            newEnemy->life = 5;
            newEnemy->isBigEnemy = true;

            if (enemyList->head == NULL)
            {
                enemyList->head = enemyList->end = newEnemy;
            }
            else
            {
                enemyList->end->next = newEnemy;
                enemyList->end = newEnemy;
            }
        }
        else
        {
            int x = rand() % (bgWidth - enemyWidth);
            int speed = minSpeed + rand() % (maxSpeed - minSpeed + 1);

            LinkList_insert(enemyList, x, 0, speed);
        }
    }
}

// 绘制敌机
void drawEnemy(LL* enemyList)
{
    if (enemyList == NULL || enemyList->head == NULL)
        return;

    Node* temp = enemyList->head;

    while (temp != NULL)
    {
        if (temp->isBigEnemy)
        {
            rectangle(
                temp->x,
                temp->y,
                temp->x + bigEnemyWidth,
                temp->y + bigEnemyHeight
            );
        }
        else
        {
            rectangle(
                temp->x,
                temp->y,
                temp->x + enemyWidth,
                temp->y + enemyHeight
            );
        }

        temp = temp->next;
    }
}

// 更新敌机位置，并删除飞出屏幕的敌机
void updateEnemy(LL* enemyList)
{
    if (enemyList == NULL || enemyList->head == NULL)
        return;

    Node* temp = enemyList->head;
    Node* next;

    while (temp != NULL)
    {
        next = temp->next;

        temp->y += temp->speed;

        if (temp->y > bgHeight)
        {
            LinkList_delete(enemyList, temp);
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
    LL* enemyList = LinkList_init();

    int lastFireTime = 0;

    srand((unsigned int)time(NULL));

    while (myPlane.alive)
    {
        cleardevice();

        moveMyPlane(&myPlane);
        fireBullet(&myPlane, bulletList, &lastFireTime);

        generateEnemy(enemyList, 50, 2, 4, false);

        updateBullet(bulletList);
        updateEnemy(enemyList);

        drawMyPlane(&myPlane);
        drawBullet(bulletList);
        drawEnemy(enemyList);

        Sleep(20);
    }

    LinkList_destroy(bulletList);
    LinkList_destroy(enemyList);
    closegraph();
}
