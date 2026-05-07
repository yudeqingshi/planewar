#include "Airplay.h"
#include <math.h>

IMAGE img[5];
IMAGE mask[5];

// 动态生成掩码并处理原图背景
void makeTransparent(IMAGE* src, IMAGE* maskImg)
{
    DWORD* srcBuf = GetImageBuffer(src);
    DWORD* maskBuf = GetImageBuffer(maskImg);
    int n = src->getwidth() * src->getheight();

    if (n <= 0)
        return;

    DWORD bg = srcBuf[0]; // 假设左上角第一个像素是背景色
    int bgR = (bg >> 16) & 0xFF;
    int bgG = (bg >> 8) & 0xFF;
    int bgB = bg & 0xFF;

    for (int i = 0; i < n; i++)
    {
        DWORD c = srcBuf[i];
        int r = (c >> 16) & 0xFF;
        int g = (c >> 8) & 0xFF;
        int b = c & 0xFF;

        if (abs(r - bgR) < 30 && abs(g - bgG) < 30 && abs(b - bgB) < 30)
        {
            maskBuf[i] = 0xFFFFFF;
            srcBuf[i] = 0x000000;
        }
        else
        {
            maskBuf[i] = 0x000000;
        }
    }
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
        putimage(plane->x, plane->y, &mask[1], SRCAND);
        putimage(plane->x, plane->y, &img[1], SRCPAINT);
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
            putimage(temp->x, temp->y, &mask[4], SRCAND);
            putimage(temp->x, temp->y, &img[4], SRCPAINT);
        }
        else
        {
            putimage(temp->x, temp->y, &mask[3], SRCAND);
            putimage(temp->x, temp->y, &img[3], SRCPAINT);
        }

        temp = temp->next;
    }
}

// 更新敌机位置并删除越界敌机
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
        putimage(temp->x, temp->y, &mask[2], SRCAND);
        putimage(temp->x, temp->y, &img[2], SRCPAINT);

        temp = temp->next;
    }
}

// 更新子弹位置并删除越界子弹
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

// 矩形碰撞检测
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return !(x1 > x2 + w2 || x1 + w1 < x2 || y1 > y2 + h2 || y1 + h1 < y2);
}

// 处理碰撞
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

// 初始化函数
void init()
{
    setbkmode(TRANSPARENT);

    loadimage(&img[0], "beijing.jpg", bgWidth, bgHeight);

    loadimage(&img[1], "myplane.jpg", myairWidth, myairHeight);
    loadimage(&mask[1], "myplane.jpg", myairWidth, myairHeight);
    makeTransparent(&img[1], &mask[1]);

    loadimage(&img[2], "bullet.jpg", bulletWidth, bulletHeight);
    loadimage(&mask[2], "bullet.jpg", bulletWidth, bulletHeight);
    makeTransparent(&img[2], &mask[2]);

    loadimage(&img[3], "enemyplane.jpg", enemyWidth, enemyHeight);
    loadimage(&mask[3], "enemyplane.jpg", enemyWidth, enemyHeight);
    makeTransparent(&img[3], &mask[3]);

    loadimage(&img[4], "bigenemy.jpg", bigEnemyWidth, bigEnemyHeight);
    loadimage(&mask[4], "bigenemy.jpg", bigEnemyWidth, bigEnemyHeight);
    makeTransparent(&img[4], &mask[4]);
}

void drawHUD(int score, int mode, int level, int timeLeft)
{
    char text[50];
    snprintf(text, 50, "Score: %d", score);
    outtextxy(10, 10, text);
}

void start()
{
    initgraph(bgWidth, bgHeight);
    init();

    MyPlane myPlane;
    initMyPlane(&myPlane);

    LL* bulletList = LinkList_init();
    LL* enemyList = LinkList_init();

    int score = 0;
    int lastFireTime = 0;

    srand((unsigned int)time(NULL));

    BeginBatchDraw();

    while (myPlane.alive)
    {
        putimage(0, 0, &img[0]);

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

        drawHUD(score, 1, 1, 0);

        FlushBatchDraw();
        Sleep(10);
    }

    EndBatchDraw();

    LinkList_destroy(bulletList);
    LinkList_destroy(enemyList);

    closegraph();
}
