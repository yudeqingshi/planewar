#pragma once
#pragma warning(disable: 4819)
#pragma warning(disable: 4996)

#include <easyx.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include "list.h"

// 窗口大小
#define bgWidth 400
#define bgHeight 600

// 我方飞机大小
#define myairWidth 30
#define myairHeight 45

// 子弹大小
#define bulletWidth 8
#define bulletHeight 8

// 敌机大小
#define enemyWidth 25
#define enemyHeight 25

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

void generateEnemy(LL* enemyList, int spawnRate = 50, int minSpeed = 2, int maxSpeed = 4);
void drawEnemy(LL* enemyList);
void updateEnemy(LL* enemyList);

void fireBullet(MyPlane* plane, LL* bulletList, int* lastFireTime);
void drawBullet(LL* bulletList);
void updateBullet(LL* bulletList);

void start();
