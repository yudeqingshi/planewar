#pragma once
#include <stdlib.h>
#include <assert.h>

// 链表节点结构体，用于管理子弹和敌机
typedef struct NODE
{
    int x;
    int y;
    int speed;
    struct NODE* next;
} Node;

// 链表结构体
typedef struct LinkList
{
    struct NODE* head;
    struct NODE* end;
} LL;

// 创建节点
Node* Node_init(int x, int y, int speed);

// 初始化链表
LL* LinkList_init();

// 尾插法插入节点
void LinkList_insert(LL* list, int x, int y, int speed);

// 删除指定节点
void LinkList_delete(LL* list, Node* node);

// 更新链表节点位置
void LinkList_update(LL* list, int direction);

// 销毁链表
void LinkList_destroy(LL* list);