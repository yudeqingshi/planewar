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

// 图片数组
extern IMAGE img[5];
extern IMAGE mask[5];

// 图片透明处理
void makeTransparent(IMAGE* src, IMAGE* maskImg);

// 游戏初始化
void init();

// 游戏启动
void start();