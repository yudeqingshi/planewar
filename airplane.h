#pragma once
#pragma warning(disable: 4819)
#pragma warning(disable: 4996)

#include <easyx.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>

// 窗口大小
#define bgWidth 400
#define bgHeight 600

// 我方飞机大小
#define myairWidth 30
#define myairHeight 45

extern IMAGE img[5];
extern IMAGE mask[5];

// 我方飞机结构体
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
void start();
