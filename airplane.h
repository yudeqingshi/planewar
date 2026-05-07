#pragma once
#pragma warning(disable: 4819) // 消除字符编码警告，防止编译时出现 C4819
#pragma warning(disable: 4996) // 消除 fopen 等文件操作的安全警告

#include<easyx.h>
#include<time.h>
#include<stdio.h>
#include<conio.h>
#include"list.h"

//窗口大小
#define bgWidth 400//窗口宽度
#define bgHeight 600//窗口高度

//我方飞机大小
#define myairWidth 30//飞机宽度
#define myairHeight 45//飞机高度

//子弹大小
#define bulletWidth 8//子弹宽度
#define bulletHeight 8//子弹高度

//敌机大小
#define enemyWidth 25//敌机宽度
#define enemyHeight 25//敌机高度

//大敌机大小
#define bigEnemyWidth 100//大敌机宽度
#define bigEnemyHeight 100//大敌机高度

//ID最大长度
#define MAX_ID_LENGTH 20

extern IMAGE img[5];//图片数组
extern IMAGE mask[5];//掩码图片数组

//排行榜条目结构体
typedef struct RankEntry
{
	int score;
	char id[MAX_ID_LENGTH];
} RankEntry;

//我方飞机结构体
typedef struct MyPlane
{
	int x;
	int y;
	int speed;
	bool alive;
}MyPlane;

void makeTransparent(IMAGE* src, IMAGE* maskImg);
void initMyPlane(MyPlane* plane);
void moveMyPlane(MyPlane* plane);
void drawMyPlane(MyPlane* plane);
void generateEnemy(LL* enemyList, int spawnRate = 50, int minSpeed = 2, int maxSpeed = 4, bool spawnBigEnemy = false);
void drawEnemy(LL* enemyList);
void updateEnemy(LL* enemyList);
void fireBullet(MyPlane* plane, LL* bulletList, int* lastFireTime);
void drawBullet(LL* bulletList);
void updateBullet(LL* bulletList);
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void handleCollision(LL* bulletList, LL* enemyList, MyPlane* plane, int* score);
void init();
void drawHUD(int score, int mode, int level, int timeLeft);
void gameOver(int score, bool isWin, int gameMode, const char* playerID);
int showStartMenu();
void start();
void getPlayerID(char* id);
bool showConfirmDialog(const char* message);
void resetRanking();
