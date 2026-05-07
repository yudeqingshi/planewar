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
	if (n <= 0) return;
	
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

		// 容差判断（由于JPG可能有压缩噪点，所以设定一个阈值）
		if (abs(r - bgR) < 30 && abs(g - bgG) < 30 && abs(b - bgB) < 30)
		{
			maskBuf[i] = 0xFFFFFF; // 掩码背景全白
			srcBuf[i] = 0x000000;  // 原图背景全黑
		}
		else
		{
			maskBuf[i] = 0x000000; // 掩码实体全黑
			// 原图实体保持不变
		}
	}
}



//初始化我方飞机
void initMyPlane(MyPlane* plane)
{
	plane->x = (bgWidth - myairWidth) / 2;
	plane->y = bgHeight - myairHeight - 10;
	plane->speed = 5;
	plane->alive = true;
}

//控制我方飞机移动
void moveMyPlane(MyPlane* plane)
{
	//获取按键状态
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

//绘制我方飞机
void drawMyPlane(MyPlane* plane)
{
	if (plane->alive)
	{
		//使用EasyX的透明绘制，黑色作为透明色
		putimage(plane->x, plane->y, &mask[1], SRCAND);
		putimage(plane->x, plane->y, &img[1], SRCPAINT);
	}
}

//生成敌机 (加入了难度参数，参数越小生成越快，默认50)
void generateEnemy(LL* enemyList, int spawnRate, int minSpeed, int maxSpeed, bool spawnBigEnemy)
{
	//随机生成敌机
	if (rand() % spawnRate == 0) // 控制敌机生成频率
	{
		// 20%的概率生成大敌机（如果允许）
		if (spawnBigEnemy && rand() % 5 == 0)
		{
			int x = rand() % (bgWidth - bigEnemyWidth);
			int speed = minSpeed / 2; // 大敌机速度较慢
			Node* newEnemy = Node_init(x, 0, speed);
			newEnemy->life = 5; // 大敌机需要5次击中才会销毁
			newEnemy->isBigEnemy = true; // 标记为大敌机
			// 将新节点插入链表
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
			// 生成普通敌机
			int x = rand() % (bgWidth - enemyWidth);
			int speed = minSpeed + rand() % (maxSpeed - minSpeed + 1); // 敌机速度
			LinkList_insert(enemyList, x, 0, speed);
		}
	}
}

//绘制敌机
void drawEnemy(LL* enemyList)
{
	if (enemyList == NULL || enemyList->head == NULL)
		return;

	Node* temp = enemyList->head;
	while (temp != NULL)
	{
		//根据生命值判断是普通敌机还是大敌机
		if (temp->life > 1) // 大敌机
		{
			//使用EasyX的透明绘制，黑色作为透明色
			putimage(temp->x, temp->y, &mask[4], SRCAND);
			putimage(temp->x, temp->y, &img[4], SRCPAINT);
		}
		else // 普通敌机
		{
			//使用EasyX的透明绘制，黑色作为透明色
			putimage(temp->x, temp->y, &mask[3], SRCAND);
			putimage(temp->x, temp->y, &img[3], SRCPAINT);
		}
		temp = temp->next;
	}
}

//更新敌机位置并移除超出屏幕的敌机
void updateEnemy(LL* enemyList)
{
	if (enemyList == NULL || enemyList->head == NULL)
		return;

	Node* temp = enemyList->head;
	Node* next;

	while (temp != NULL)
	{
		next = temp->next;
		//更新位置
		temp->y += temp->speed;
		//检查是否超出屏幕
		if (temp->y > bgHeight)
		{
			LinkList_delete(enemyList, temp);
		}
		temp = next;
	}
}

//发射子弹
void fireBullet(MyPlane* plane, LL* bulletList, int* lastFireTime)
{
	//空格键发射子弹
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		//获取当前时间
		int currentTime = GetTickCount();
		//控制发射间隔，避免连续发射
		if (currentTime - *lastFireTime > 200) // 200毫秒的发射间隔
		{
			//计算子弹发射位置（从飞机中心发射）
			int bulletX = plane->x + myairWidth / 2 - bulletWidth / 2;
			int bulletY = plane->y - bulletHeight;
			LinkList_insert(bulletList, bulletX, bulletY, 8); // 子弹速度为8
			//更新上次发射时间
			*lastFireTime = currentTime;
		}
	}
}

//绘制子弹
void drawBullet(LL* bulletList)
{
	if (bulletList == NULL || bulletList->head == NULL)
		return;

	Node* temp = bulletList->head;
	while (temp != NULL)
	{
		//使用EasyX的透明绘制，黑色作为透明色
		putimage(temp->x, temp->y, &mask[2], SRCAND);
		putimage(temp->x, temp->y, &img[2], SRCPAINT);
		temp = temp->next;
	}
}

//更新子弹位置并移除超出屏幕的子弹
void updateBullet(LL* bulletList)
{
	if (bulletList == NULL || bulletList->head == NULL)
		return;

	Node* temp = bulletList->head;
	Node* next;

	while (temp != NULL)
	{
		next = temp->next;
		//更新位置
		temp->y -= temp->speed;
		//检查是否超出屏幕
		if (temp->y < -bulletHeight)
		{
			LinkList_delete(bulletList, temp);
		}
		temp = next;
	}
}

//检测碰撞
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return !(x1 > x2 + w2 || x1 + w1 < x2 || y1 > y2 + h2 || y1 + h1 < y2);
}

//处理碰撞检测
void handleCollision(LL* bulletList, LL* enemyList, MyPlane* plane, int* score)
{
	//检测子弹与敌机的碰撞
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
				//根据isBigEnemy判断是普通敌机还是大敌机，使用不同的宽度和高度
				int currentEnemyWidth = (enemy->isBigEnemy) ? bigEnemyWidth : enemyWidth;
				int currentEnemyHeight = (enemy->isBigEnemy) ? bigEnemyHeight : enemyHeight;
					
					//检测碰撞
					if (checkCollision(bullet->x, bullet->y, bulletWidth, bulletHeight, 
						enemy->x, enemy->y, currentEnemyWidth, currentEnemyHeight))
					{
						//移除子弹
						LinkList_delete(bulletList, bullet);
						
						//减少敌机生命值
						enemy->life--;
						
						//如果敌机生命值为0，移除敌机并增加分数
					if (enemy->life <= 0)
					{
						//保存敌机类型
						bool isBigEnemy = enemy->isBigEnemy;
						//移除敌机
						LinkList_delete(enemyList, enemy);
						//增加分数，大敌机得分更高
						if (isBigEnemy)
						{
							(*score) += 50; // 大敌机得分50分
						}
						else
						{
							(*score) += 10; // 普通敌机得分10分
						}
					}
						break;
					}
				enemy = enemyNext;
			}
			bullet = bulletNext;
		}
	}

	//检测敌机与我方飞机的碰撞
	if (plane->alive && enemyList != NULL && enemyList->head != NULL)
	{
		Node* enemy = enemyList->head;
		Node* enemyNext;

		while (enemy != NULL)
		{
			enemyNext = enemy->next;
			//根据生命值判断是普通敌机还是大敌机，使用不同的宽度和高度
			int currentEnemyWidth = (enemy->life > 1) ? bigEnemyWidth : enemyWidth;
			int currentEnemyHeight = (enemy->life > 1) ? bigEnemyHeight : enemyHeight;
			
			//检测碰撞
			if (checkCollision(plane->x, plane->y, myairWidth, myairHeight, 
				enemy->x, enemy->y, currentEnemyWidth, currentEnemyHeight))
			{
				//我方飞机被击中
				plane->alive = false;
				//移除敌机
				LinkList_delete(enemyList, enemy);
				break;
			}
			enemy = enemyNext;
		}
	}
}

//初始化函数
void init()
{
	// 设置文字背景模式为透明，去掉文字后面的黑框
	setbkmode(TRANSPARENT);

	//加载图片
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
	
	//加载大敌机图片
	loadimage(&img[4], "bigenemy.jpg", bigEnemyWidth, bigEnemyHeight);
	loadimage(&mask[4], "bigenemy.jpg", bigEnemyWidth, bigEnemyHeight);
	makeTransparent(&img[4], &mask[4]);
}



//绘制分数和关卡信息
void drawHUD(int score, int mode, int level, int timeLeft)
{
	char text[50];
	if (mode == 1)
	{
		snprintf(text, 50, "Score: %d", score);
		outtextxy(10, 10, text);
	}
	else if (mode == 2)
	{
		snprintf(text, 50, "Level: %d  Time: %ds", level, timeLeft);
		outtextxy(10, 10, text);
	}
}

//更新排行榜数据（无尽模式专用）
void updateAndSaveRanking(int newScore, const char* playerID)
{
	RankEntry entries[21] = { 0 };
	int count = 0;
	
	// 从文件中读取已有的分数和ID
	FILE* fp = fopen("rank.txt", "r");
	if (fp)
	{
		while (count < 20 && fscanf(fp, "%d %s", &entries[count].score, entries[count].id) == 2)
		{
			count++;
		}
		fclose(fp);
	}
	
	// 加上新分数和ID
	entries[count].score = newScore;
	snprintf(entries[count].id, MAX_ID_LENGTH, "%s", playerID);
	count++;

	// 降序排序
	for (int i = 0; i < count - 1; i++)
	{
		for (int j = i + 1; j < count; j++)
		{
			if (entries[i].score < entries[j].score)
			{
				RankEntry temp = entries[i];
				entries[i] = entries[j];
				entries[j] = temp;
			}
		}
	}

	// 截断至最多 20 个
	if (count > 20) count = 20;

	// 重新写回文件
	fp = fopen("rank.txt", "w");
	if (fp)
	{
		for (int i = 0; i < count; i++)
		{
			fprintf(fp, "%d %s\n", entries[i].score, entries[i].id);
		}
		fclose(fp);
	}
}

// 显示排行榜画面
void showRanking()
{
	bool isBack = false;
	ExMessage msg;

	int backBtnX = bgWidth / 2 - 50;
	int backBtnY = bgHeight - 60;
	int backBtnW = 100;
	int backBtnH = 40;

	int resetBtnX = bgWidth / 2 - 50;
	int resetBtnY = bgHeight - 110;
	int resetBtnW = 100;
	int resetBtnH = 40;

	BeginBatchDraw();
	while (!isBack)
	{
		// 重新读取排行榜数据
		RankEntry entries[20] = { 0 };
		int count = 0;
		FILE* fp = fopen("rank.txt", "r");
		if (fp)
		{
			while (count < 20 && fscanf(fp, "%d %s", &entries[count].score, entries[count].id) == 2)
			{
				count++;
			}
			fclose(fp);
		}

		putimage(0, 0, &img[0]); // 绘制背景

		// 标题
		settextcolor(BLACK);
		settextstyle(30, 0, "Consolas");
		outtextxy(bgWidth / 2 - textwidth("TOP 20 RANKING") / 2, 20, "TOP 20 RANKING");

		// 分数列表
		settextstyle(18, 0, "Consolas");
		for (int i = 0; i < count; i++)
		{
			char buf[100];
			snprintf(buf, 100, "No.%2d: %d %s", i + 1, entries[i].score, entries[i].id);
			outtextxy(bgWidth / 2 - textwidth(buf) / 2, 60 + i * 22, buf);
		}

		// Reset 按钮
		setlinecolor(BLACK);
		setfillcolor(RED);
		fillrectangle(resetBtnX, resetBtnY, resetBtnX + resetBtnW, resetBtnY + resetBtnH);
		settextcolor(WHITE);
		settextstyle(20, 0, "Consolas");
		outtextxy(resetBtnX + (resetBtnW - textwidth("Reset")) / 2, resetBtnY + (resetBtnH - textheight("Reset")) / 2, "Reset");

		// Back 按钮
		setlinecolor(BLACK);
		setfillcolor(LIGHTGRAY);
		fillrectangle(backBtnX, backBtnY, backBtnX + backBtnW, backBtnY + backBtnH);
		settextcolor(BLACK);
		settextstyle(20, 0, "Consolas");
		outtextxy(backBtnX + (backBtnW - textwidth("Back")) / 2, backBtnY + (backBtnH - textheight("Back")) / 2, "Back");

		FlushBatchDraw();

		while (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				// 点击 Reset 按钮
				if (msg.x >= resetBtnX && msg.x <= resetBtnX + resetBtnW && msg.y >= resetBtnY && msg.y <= resetBtnY + resetBtnH)
				{
					// 显示确认对话框
					if (showConfirmDialog("Are you sure?"))
					{
						// 重置排行榜
						resetRanking();
					}
				}
				// 点击 Back 按钮
				else if (msg.x >= backBtnX && msg.x <= backBtnX + backBtnW && msg.y >= backBtnY && msg.y <= backBtnY + backBtnH)
				{
					isBack = true;
				}
			}
		}
		Sleep(10);
	}
	EndBatchDraw();
}

//游戏结束画面
void gameOver(int score, bool isWin, int gameMode, const char* playerID)
{
	// 只有无尽模式记录排行榜
	if (gameMode == 1)
	{
		// 保存分数和ID到排行榜
		updateAndSaveRanking(score, playerID);
	}

	bool waiting = true;
	ExMessage msg;

	// 外层大循环：从排行榜返回后还能重新画出游戏结束画面
	BeginBatchDraw();
	while (waiting)
	{
		putimage(0, 0, &img[0]); // 重新绘制背景
		
		char gameOverText[50];
		if (isWin)
		{
			snprintf(gameOverText, 50, "You Win! All Levels Cleared!");
		}
		else
		{
			snprintf(gameOverText, 50, "Game Over! Score: %d", score);
		}
		settextcolor(WHITE);
		settextstyle(20, 0, "Consolas");
		outtextxy(bgWidth / 2 - textwidth(gameOverText) / 2, bgHeight / 2 - 60, gameOverText);
		
		// 绘制 "Back to the Menu" 按钮
		int btnX = bgWidth / 2 - 100;
		int btnY = bgHeight / 2 + 10;
		int btnW = 200;
		int btnH = 40;

		setlinecolor(BLACK);
		setfillcolor(LIGHTGRAY);
		fillrectangle(btnX, btnY, btnX + btnW, btnY + btnH);

		settextcolor(BLACK);
		settextstyle(20, 0, "Consolas");
		int textX = btnX + (btnW - textwidth("Back to the Menu")) / 2;
		int textY = btnY + (btnH - textheight("Back to the Menu")) / 2;
		outtextxy(textX, textY, "Back to the Menu");

		// 如果是无尽模式，多画一个 "Ranking" 按钮
		int rankBtnX = bgWidth / 2 - 100;
		int rankBtnY = bgHeight / 2 + 70;
		int rankBtnW = 200;
		int rankBtnH = 40;

		if (gameMode == 1)
		{
			fillrectangle(rankBtnX, rankBtnY, rankBtnX + rankBtnW, rankBtnY + rankBtnH);
			outtextxy(rankBtnX + (rankBtnW - textwidth("Ranking")) / 2, rankBtnY + (rankBtnH - textheight("Ranking")) / 2, "Ranking");
		}

		FlushBatchDraw(); // 确保文字和按钮被绘制到屏幕上
		
		// 清空积压事件
		while (peekmessage(&msg)) {} 

		// 等待玩家点击按钮
		bool actionTaken = false;
		while (!actionTaken)
		{
			while (peekmessage(&msg))
			{
				// 仅监听鼠标左键点击
				if (msg.message == WM_LBUTTONDOWN)
				{
					// 判断点击位置是否在 "Back to the Menu" 按钮范围内
					if (msg.x >= btnX && msg.x <= btnX + btnW &&
						msg.y >= btnY && msg.y <= btnY + btnH)
					{
						waiting = false;
						actionTaken = true;
						break;
					}
					// 判断点击位置是否在 "Ranking" 按钮范围内
					else if (gameMode == 1 && msg.x >= rankBtnX && msg.x <= rankBtnX + rankBtnW &&
							 msg.y >= rankBtnY && msg.y <= rankBtnY + rankBtnH)
					{
						showRanking();
						actionTaken = true; // 打断内层循环，重新画 Game Over
						break;
					}
				}
			}
			Sleep(10);
		}
	}
	EndBatchDraw();
	
	// 恢复游戏运行时的字体样式
	settextcolor(WHITE);
	settextstyle(16, 0, "Consolas");
}

// 显示游戏封面并等待开始点击，返回选择的模式：1 为 Endless, 2 为 Level
int showStartMenu()
{
	int selectedMode = 0;
	ExMessage msg; // 恢复使用最新版 EasyX 的鼠标消息结构体

	// Endless Mode 按钮的坐标和尺寸
	int btn1X = bgWidth / 2 - 80;
	int btn1Y = bgHeight / 2 + 20;
	int btn1W = 160;
	int btn1H = 40;

	// Level Mode 按钮的坐标和尺寸
	int btn2X = bgWidth / 2 - 80;
	int btn2Y = bgHeight / 2 + 80;
	int btn2W = 160;
	int btn2H = 40;

	BeginBatchDraw();
	while (selectedMode == 0)
	{
		// 绘制背景
		putimage(0, 0, &img[0]);

		// 绘制标题 "Plane Wars"
		settextcolor(BLACK);
		settextstyle(40, 0, "Consolas"); // 设置较大的字体
		int titleX = bgWidth / 2 - textwidth("Plane Wars") / 2;
		int titleY = bgHeight / 2 - 100;
		outtextxy(titleX, titleY, "Plane Wars");

		// 绘制 Endless Mode 按钮框
		setlinecolor(BLACK);
		setfillcolor(LIGHTGRAY);
		fillrectangle(btn1X, btn1Y, btn1X + btn1W, btn1Y + btn1H);

		// 绘制 Endless Mode 按钮文字
		settextstyle(20, 0, "Consolas");
		int text1X = btn1X + (btn1W - textwidth("Endless Mode")) / 2;
		int text1Y = btn1Y + (btn1H - textheight("Endless Mode")) / 2;
		outtextxy(text1X, text1Y, "Endless Mode");

		// 绘制 Level Mode 按钮框
		fillrectangle(btn2X, btn2Y, btn2X + btn2W, btn2Y + btn2H);

		// 绘制 Level Mode 按钮文字
		int text2X = btn2X + (btn2W - textwidth("Level Mode")) / 2;
		int text2Y = btn2Y + (btn2H - textheight("Level Mode")) / 2;
		outtextxy(text2X, text2Y, "Level Mode");

		FlushBatchDraw();

		// 处理鼠标消息
		while (peekmessage(&msg))
		{
			// 如果左键按下
			if (msg.message == WM_LBUTTONDOWN)
			{
				// 判断点击位置是否在 Endless Mode 按钮范围内
				if (msg.x >= btn1X && msg.x <= btn1X + btn1W &&
					msg.y >= btn1Y && msg.y <= btn1Y + btn1H)
				{
					selectedMode = 1;
				}
				// 判断点击位置是否在 Level Mode 按钮范围内
				else if (msg.x >= btn2X && msg.x <= btn2X + btn2W &&
					msg.y >= btn2Y && msg.y <= btn2Y + btn2H)
				{
					selectedMode = 2;
				}
			}
		}

		Sleep(10);
	}
	EndBatchDraw();
	
	// 恢复游戏运行时的字体样式
	settextcolor(WHITE);
	settextstyle(16, 0, "Consolas");

	return selectedMode;
}

//显示确认对话框
bool showConfirmDialog(const char* message)
{
	bool isConfirmed = false;
	bool isDone = false;
	ExMessage msg;

	// 按钮位置
	int yesBtnX = bgWidth / 2 - 150;
	int yesBtnY = bgHeight / 2 + 50;
	int yesBtnW = 120;
	int yesBtnH = 40;

	int noBtnX = bgWidth / 2 + 30;
	int noBtnY = bgHeight / 2 + 50;
	int noBtnW = 120;
	int noBtnH = 40;

	BeginBatchDraw();
	while (!isDone)
	{
		// 半透明背景
		setfillcolor(RGB(255, 255, 255));
		solidrectangle(0, 0, bgWidth, bgHeight);

		// 对话框背景
		setlinecolor(BLACK);
		setfillcolor(WHITE);
		fillrectangle(bgWidth / 2 - 200, bgHeight / 2 - 100, bgWidth / 2 + 200, bgHeight / 2 + 100);

		// 消息文本
		settextcolor(BLACK);
		settextstyle(20, 0, "Consolas");
		outtextxy(bgWidth / 2 - textwidth(message) / 2, bgHeight / 2 - 30, message);

		// YES按钮
		setlinecolor(BLACK);
		setfillcolor(GREEN);
		fillrectangle(yesBtnX, yesBtnY, yesBtnX + yesBtnW, yesBtnY + yesBtnH);
		settextcolor(WHITE);
		settextstyle(18, 0, "Consolas");
		outtextxy(yesBtnX + (yesBtnW - textwidth("YESYESYES!!!")) / 2, yesBtnY + (yesBtnH - textheight("YESYESYES!!!")) / 2, "YESYESYES!!!");

		// NO按钮
		setlinecolor(BLACK);
		setfillcolor(RED);
		fillrectangle(noBtnX, noBtnY, noBtnX + noBtnW, noBtnY + noBtnH);
		settextcolor(WHITE);
		settextstyle(18, 0, "Consolas");
		outtextxy(noBtnX + (noBtnW - textwidth("NONONO!!!")) / 2, noBtnY + (noBtnH - textheight("NONONO!!!")) / 2, "NONONO!!!");

		FlushBatchDraw();

		// 处理鼠标消息
		while (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				// 点击YES按钮
				if (msg.x >= yesBtnX && msg.x <= yesBtnX + yesBtnW &&
					msg.y >= yesBtnY && msg.y <= yesBtnY + yesBtnH)
				{
					isConfirmed = true;
					isDone = true;
				}
				// 点击NO按钮
				else if (msg.x >= noBtnX && msg.x <= noBtnX + noBtnW &&
					msg.y >= noBtnY && msg.y <= noBtnY + noBtnH)
				{
					isConfirmed = false;
					isDone = true;
				}
			}
		}

		Sleep(10);
	}
	EndBatchDraw();

	return isConfirmed;
}

//重置排行榜
void resetRanking()
{
	// 打开文件并清空内容
	FILE* fp = fopen("rank.txt", "w");
	if (fp)
	{
		fclose(fp);
	}
}

//获取玩家ID
void getPlayerID(char* id)
{
	char input[MAX_ID_LENGTH] = { 0 };
	int inputIndex = 0;
	bool isDone = false;
	ExMessage msg;

	BeginBatchDraw();
	while (!isDone)
	{
		putimage(0, 0, &img[0]); // 绘制背景

		// 标题
		settextcolor(BLACK);
		settextstyle(30, 0, "Consolas");
		outtextxy(bgWidth / 2 - textwidth("Enter your ID") / 2, 100, "Enter your ID");

		// 输入框
		setlinecolor(BLACK);
		setfillcolor(WHITE);
		fillrectangle(bgWidth / 2 - 100, 200, bgWidth / 2 + 100, 240);

		// 显示输入的ID
		settextcolor(BLACK);
		settextstyle(20, 0, "Consolas");
		outtextxy(bgWidth / 2 - 95, 205, input);

		// 提示信息
		settextcolor(BLACK);
		settextstyle(16, 0, "Consolas");
		outtextxy(bgWidth / 2 - textwidth("Press Enter to confirm") / 2, 280, "Press Enter to confirm");

		FlushBatchDraw();

		// 处理所有消息
		while (peekmessage(&msg))
		{
			if (msg.message == WM_CHAR)
			{
				if (msg.vkcode == VK_RETURN) // 回车键确认
				{
					isDone = true;
				}
				else if (msg.vkcode == VK_BACK) // 退格键删除
				{
					if (inputIndex > 0)
					{
						input[--inputIndex] = '\0';
					}
				}
				else if (inputIndex < MAX_ID_LENGTH - 1 && isalnum(msg.vkcode)) // 只允许字母和数字
				{
					input[inputIndex++] = msg.vkcode;
					input[inputIndex] = '\0';
				}
			}
		}

		Sleep(10);
	}
	EndBatchDraw();

	// 复制输入的ID到参数
	snprintf(id, MAX_ID_LENGTH, "%s", input);
}

//所有功能从开始函数开始
void start()
{
	initgraph(bgWidth, bgHeight);
	init();

	// 外层大循环，支持游戏结束返回主菜单
	while (true)
	{
		// 显示游戏封面，获取选择的模式
		int gameMode = showStartMenu();

		// 存储玩家ID
		char playerID[MAX_ID_LENGTH] = "Player";
		
		// 如果是无尽模式，要求玩家输入ID
		if (gameMode == 1)
		{
			getPlayerID(playerID);
		}

		//初始化游戏变量
		MyPlane myPlane;
		initMyPlane(&myPlane);

		LL* bulletList = LinkList_init();
		LL* enemyList = LinkList_init();

		int score = 0;
		bool gameover = false;
		bool isWin = false;
		int lastFireTime = 0; // 上次发射子弹的时间

		// 关卡模式相关的变量
		int currentLevel = 1;
		int levelStartTime = GetTickCount(); // 记录当前关卡开始的时间
		int survivalTime = 20; // 每一关需要存活的时间（秒）

		//设置随机种子
		srand((unsigned int)time(NULL));

		//开始批量绘制（双缓冲）
		BeginBatchDraw();

		//游戏主循环
		while (!gameover)
		{
			//绘制背景
			putimage(0, 0, &img[0]);

			//处理用户输入
			moveMyPlane(&myPlane);
			fireBullet(&myPlane, bulletList, &lastFireTime);

			int currentTime = GetTickCount();
			int elapsedSeconds = (currentTime - levelStartTime) / 1000;
			int timeLeft = survivalTime - elapsedSeconds;

			// 生成敌机，根据模式调整难度
			if (gameMode == 1) // 无尽模式
			{
				// 随着分数增加，稍微增加难度
				int spawnRate = 50 - (score / 100);
				if (spawnRate < 10) spawnRate = 10;
				int maxSpeed = 4 + (score / 200);
				// 得分达到100分后开始生成大敌机
				bool spawnBigEnemy = (score >= 100);
				generateEnemy(enemyList, spawnRate, 2, maxSpeed, spawnBigEnemy);
			}
			else if (gameMode == 2) // 闯关模式
			{
				// 难度随关卡增加
				int spawnRate = 50 - (currentLevel * 4); // 关卡越高，生成越快
				if (spawnRate < 5) spawnRate = 5;
				int minSpeed = 2 + (currentLevel / 3);
			int maxSpeed = 4 + (currentLevel / 2);
				// 第3关开始生成大敌机
				bool spawnBigEnemy = (currentLevel >= 3);
				generateEnemy(enemyList, spawnRate, minSpeed, maxSpeed, spawnBigEnemy);

				// 检查是否通过当前关卡
				if (timeLeft <= 0)
				{
					currentLevel++;
					if (currentLevel > 10)
					{
						// 通关了10关，游戏胜利
						gameover = true;
						isWin = true;
					}
					else
					{
						// 进入下一关，重置时间，清空敌机和子弹
						levelStartTime = GetTickCount();
						LinkList_destroy(enemyList);
						LinkList_destroy(bulletList);
						enemyList = LinkList_init();
						bulletList = LinkList_init();
						
						// 提示进入下一关
						settextcolor(WHITE);
						settextstyle(30, 0, "Consolas");
						char levelText[50];
						snprintf(levelText, 50, "Level %d Start!", currentLevel);
						outtextxy(bgWidth / 2 - textwidth(levelText) / 2, bgHeight / 2 - 15, levelText);
						FlushBatchDraw();
						Sleep(1500); // 暂停1.5秒给玩家准备时间
						settextstyle(16, 0, "Consolas");
					}
				}
			}

			//更新所有游戏元素的位置
			updateBullet(bulletList);
			updateEnemy(enemyList);

			//处理碰撞检测
			handleCollision(bulletList, enemyList, &myPlane, &score);

			//检查游戏是否结束
			if (!myPlane.alive)
			{
				gameover = true;
			}

			//绘制所有游戏元素
			drawMyPlane(&myPlane);
			drawBullet(bulletList);
			drawEnemy(enemyList);

			//显示分数或关卡HUD
			drawHUD(score, gameMode, currentLevel, timeLeft > 0 ? timeLeft : 0);

			//结束批量绘制并显示到屏幕
			FlushBatchDraw();

			//控制游戏帧率
			Sleep(10);
		}

		//游戏结束处理
		gameOver(score, isWin, gameMode, playerID);

		//结束双缓冲
		EndBatchDraw();

		//释放资源，清理上一局数据准备下一局
		LinkList_destroy(bulletList);
		LinkList_destroy(enemyList);
	}

	closegraph();
}
