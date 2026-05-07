#pragma once
#pragma warning(disable: 4819)
#pragma warning(disable: 4996)

#include <easyx.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include "list.h"

#define bgWidth 400
#define bgHeight 600

#define myairWidth 30
#define myairHeight 45

#define bulletWidth 8
#define bulletHeight 8

#define enemyWidth 25
#define enemyHeight 25

#define bigEnemyWidth 100
#define bigEnemyHeight 100

extern IMAGE img[5];
extern IMAGE mask[5];

typedef struct MyPlane
{
    int x;
    int y;
    int speed;
    bool alive;
} MyPlane;

void makeTransparent(IMAGE* src, IMAGE* maskImg);
void init();

void initMyPlane(MyPlane* plane);
void moveMyPlane(MyPlane* plane);
void drawMyPlane(MyPlane* plane);

void generateEnemy(LL* enemyList, int spawnRate = 50, int minSpeed = 2, int maxSpeed = 4, bool spawnBigEnemy = false);
void drawEnemy(LL* enemyList);
void updateEnemy(LL* enemyList);

void fireBullet(MyPlane* plane, LL* bulletList, int* lastFireTime);
void drawBullet(LL* bulletList);
void updateBullet(LL* bulletList);

bool checkCollision(int x1, int y1, int w1, int h1,
                    int x2, int y2, int w2, int h2);

void handleCollision(LL* bulletList, LL* enemyList, MyPlane* plane, int* score);

void start();
