//
// Created by Yoshi on 2024/1/20.
//

#ifndef LIST_ENTRY_LIST_ENTRY_H
#define LIST_ENTRY_LIST_ENTRY_H

#include "stdint.h"
#include "stdio.h"

/*****************************************************************************
 * 第一个参数: ptr是结构体中某个成员的地址
 * 第二个参数: type是结构体类型
 * 第三个参数: member是结构体中成员的名字
 ******************************************************************************/

#define list_entry(ptr, type, member) \
        container_of(ptr, type, member)

/**
 * @define: container_of
 * @brief:  通过获取一个结构体的成员变量, 从而得到结构体的地址
 * @arguments:
 *  1.  __mptr(ptr): 它的类型由typeof获得, 作用是指向type结构体中成员member的一个指针
 *  2.  type:   传入结构体的模板
 *  3.  member: 传入要复制的结构体变量的成员变量名称
 * @return: 返回一个地址值
 *
 * 位于 /usr/src/include/linux/kernel.h
 */

#define container_of(ptr, type, member)({ \
    const typeof ( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type, member) ); \
    })

/**
 * @define: offsetof
 * @brief: 用于计算TYPE结构体中, 成员member的地址偏移量
 * @argument:
 *  1. TYPE: 传入结构体的模板
 *  2. member: 传入要复制的结构体变量的成员变量名称
 * @return: 返回偏移量的大小
 *
 * 位于 include/linux/stddef.h
 */

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * 链表的链域, 区别于数据域
 * */
struct list_node
{
    struct list_node *next;
    struct list_node *prev;
};

typedef struct list_node list_t;

struct list_objectExample{
    //数据域
    uint8_t data1;
    uint32_t data2;
    char* string;
    //链域
    list_t list;
};

/*********************************链表的操作*************************************/

/**
 * @brief: 链表头的初始化
 */
#define LIST_HEAD_INIT(name)    {&(name), &(name)}

#define LIST_HEAD(name) \
    struct list_t name = LIST_HEAD_INIT(name)

/**
 * @brief: 内联函数版的链表初始化, 用于运行中的初始化
 */

static inline void INIT_LIST_HEAD(list_t* list){
    list->next = list;
    list->prev = list;
}

/**
 *  Insert a new entry between two known consecutive entries.
 *
 *  This is only for internal list manipulation where we know
 *  the prev/next entries already!
 */

static inline void __list_add(list_t* new, list_t *prev, list_t* next){
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 *  delete a list node between two known consecutive entries.
 */

static inline void __list_del(list_t* prev, list_t* next){
    next->prev = prev;
    prev->next = next;
}

/**
 *  list_add    -   add a new entry
 *  @new: new entry to be added
 *  @head: list head to add it after
 *
 *  Insert a new entry after the specified head.
 *  This is good for implementing stacks.
 */

static inline void list_add(list_t* new, list_t* head){
    __list_add(new, head, head->next);
}

/**
 *  list_add_tail   -   add a new entry
 *  @new: new entry to be added
 *  @head: list head to add it before
 *
 *  Insert a new entry before the specified head
 *  This is useful for implementing queues.
 */

static inline void list_add_tail(list_t* new, list_t* head){
    __list_add(new, head->prev, head);
}

/**
 *  list_del   -   delete a known entry
 *  @entry: the element to delete from the list.
 */

static inline void list_del(list_t* entry){
    __list_del(entry->prev, entry->next);

    //在linux内核中, LIST_POSION被定义为了两个正常情况下不可访问的地址, 避免错误发生
//    entry->next = LIST_POSION1;
//    entry->prev = LIST_POSION2;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(list_t *entry)
{
    list_del(entry);
    INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(list_t* list, list_t* head){
    __list_del(list->prev, list->next);
    list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(list_t *list,
                                  list_t *head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const list_t *head)
{
    return head->next == head;
}

/**
 * list_for_each_entry - iterate over list of given type
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_struct within the struct.
 *
 * 正向遍历整个链表, 得到list, 然后得到其宿主结构的地址
 */
#define list_for_each_entry(pos, head, member)    \
 for (pos = list_entry((head)->next, typeof(*pos), member); \
      &pos->member != (head);  \
      pos = list_entry(pos->member.next, typeof(*pos), member))

/**
* list_for_each_entry_reverse - iterate backwards over list of given type.
* @pos: the type * to use as a loop cursor.
* @head: the head for your list.
* @member: the name of the list_struct within the struct.
 *
 * 反向遍历整个链表, 得到list, 然后得到其宿主结构的地址
*/
#define list_for_each_entry_reverse(pos, head, member)   \
   for (pos = list_entry((head)->prev, typeof(*pos), member); \
        &pos->member != (head);  \
        pos = list_entry(pos->member.prev, typeof(*pos), member))

/****************************************************************************/

#endif //LIST_ENTRY_LIST_ENTRY_H
