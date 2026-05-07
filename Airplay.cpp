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

// 矩形碰撞检测
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return !(x1 > x2 + w2 || x1 + w1 < x2 || y1 > y2 + h2 || y1 + h1 < y2);
}

// 处理子弹与敌机、敌机与玩家飞机的碰撞
void handleCollision(LL* bulletList, LL* enemyList, MyPlane* plane, int* score)
{
    if (bulletList != NULL && bulletList->head != NULL && enemyList != NULL && enemyList->head != NULL)
    {
        Node* bullet = bulletList->head;
        Node* bulletNext;

        while (bullet != NULL)
        {
            bulletNext = bullet->next;

            Node* enemy = enemyList->head;
            Node* enemyNext;

            while (enemy != NULL)
            {
                enemyNext = enemy->next;

                int currentEnemyWidth = enemy->isBigEnemy ? bigEnemyWidth : enemyWidth;
                int currentEnemyHeight = enemy->isBigEnemy ? bigEnemyHeight : enemyHeight;

                if (checkCollision(
                    bullet->x,
                    bullet->y,
                    bulletWidth,
                    bulletHeight,
                    enemy->x,
                    enemy->y,
                    currentEnemyWidth,
                    currentEnemyHeight
                ))
                {
                    LinkList_delete(bulletList, bullet);

                    enemy->life--;

                    if (enemy->life <= 0)
                    {
                        bool isBigEnemy = enemy->isBigEnemy;

                        LinkList_delete(enemyList, enemy);

                        if (isBigEnemy)
                        {
                            (*score) += 50;
                        }
                        else
                        {
                            (*score) += 10;
                        }
                    }

                    break;
                }

                enemy = enemyNext;
            }

            bullet = bulletNext;
        }
    }

    if (plane->alive && enemyList != NULL && enemyList->head != NULL)
    {
        Node* enemy = enemyList->head;
        Node* enemyNext;

        while (enemy != NULL)
        {
            enemyNext = enemy->next;

            int currentEnemyWidth = enemy->isBigEnemy ? bigEnemyWidth : enemyWidth;
            int currentEnemyHeight = enemy->isBigEnemy ? bigEnemyHeight : enemyHeight;

            if (checkCollision(
                plane->x,
                plane->y,
                myairWidth,
                myairHeight,
                enemy->x,
                enemy->y,
                currentEnemyWidth,
                currentEnemyHeight
            ))
            {
                plane->alive = false;
                LinkList_delete(enemyList, enemy);
                break;
            }

            enemy = enemyNext;
        }
    }
}

void start()
{
    init();

    MyPlane myPlane;
    initMyPlane(&myPlane);

    LL* bulletList = LinkList_init();
    LL* enemyList = LinkList_init();

    int score = 0;
    int lastFireTime = 0;

    srand((unsigned int)time(NULL));

    while (myPlane.alive)
    {
        cleardevice();

        moveMyPlane(&myPlane);
        fireBullet(&myPlane, bulletList, &lastFireTime);

        bool spawnBigEnemy = (score >= 100);
        generateEnemy(enemyList, 50, 2, 4, spawnBigEnemy);

        updateBullet(bulletList);
        updateEnemy(enemyList);

        handleCollision(bulletList, enemyList, &myPlane, &score);

        drawMyPlane(&myPlane);
        drawBullet(bulletList);
        drawEnemy(enemyList);

        char text[50];
        snprintf(text, 50, "Score: %d", score);
        outtextxy(10, 10, text);

        Sleep(20);
    }

    LinkList_destroy(bulletList);
    LinkList_destroy(enemyList);
    closegraph();
}
