#include "list.h"

// 创建链表节点
Node* Node_init(int x, int y, int speed)
{
    Node* temp = (Node*)malloc(sizeof(Node));
    assert(temp);

    temp->x = x;
    temp->y = y;
    temp->speed = speed;
    temp->next = NULL;

    return temp;
}

// 初始化链表
LL* LinkList_init()
{
    LL* list = (LL*)malloc(sizeof(LL));
    assert(list);

    list->head = NULL;
    list->end = NULL;

    return list;
}

// 尾插法插入节点
void LinkList_insert(LL* list, int x, int y, int speed)
{
    if (list == NULL)
        return;

    Node* newNode = Node_init(x, y, speed);

    if (list->head == NULL)
    {
        list->head = newNode;
        list->end = newNode;
    }
    else
    {
        list->end->next = newNode;
        list->end = newNode;
    }
}

// 删除指定节点
void LinkList_delete(LL* list, Node* node)
{
    if (list == NULL || list->head == NULL || node == NULL)
        return;

    if (node == list->head)
    {
        list->head = node->next;

        if (list->head == NULL)
            list->end = NULL;

        free(node);
        return;
    }

    Node* prev = list->head;

    while (prev->next != NULL && prev->next != node)
    {
        prev = prev->next;
    }

    if (prev->next == node)
    {
        prev->next = node->next;

        if (node == list->end)
            list->end = prev;

        free(node);
    }
}

// direction: 1 上，2 下，3 左，4 右
void LinkList_update(LL* list, int direction)
{
    if (list == NULL || list->head == NULL)
        return;

    Node* temp = list->head;

    while (temp != NULL)
    {
        switch (direction)
        {
        case 1:
            temp->y -= temp->speed;
            break;
        case 2:
            temp->y += temp->speed;
            break;
        case 3:
            temp->x -= temp->speed;
            break;
        case 4:
            temp->x += temp->speed;
            break;
        }

        temp = temp->next;
    }
}

// 销毁链表，释放内存
void LinkList_destroy(LL* list)
{
    if (list == NULL)
        return;

    Node* temp = list->head;
    Node* next;

    while (temp != NULL)
    {
        next = temp->next;
        free(temp);
        temp = next;
    }

    free(list);
}