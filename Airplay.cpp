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

void start()
{
    init();

    while (true)
    {
        cleardevice();
        Sleep(20);
    }

    closegraph();
}