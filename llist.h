#pragma once
#include <stdlib.h>
#include <assert.h>

// 链表节点结构体
typedef struct NODE
{
    int x;
    int y;
    int speed;
    int life;
    bool isBigEnemy;
    struct NODE* next;
} Node;

// 链表结构体
typedef struct LinkList
{
    struct NODE* head;
    struct NODE* end;
} LL;

Node* Node_init(int x, int y, int speed);
LL* LinkList_init();
void LinkList_insert(LL* list, int x, int y, int speed);
void LinkList_delete(LL* list, Node* node);
void LinkList_update(LL* list, int direction);
void LinkList_destroy(LL* list);
