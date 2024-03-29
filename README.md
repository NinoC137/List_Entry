# List_Entry 侵入式链表

主要参考了linux内核中的list.h

实现了基本的增删查改, 及对应的api使用范例



## 侵入式链表是什么?

链表可被分为侵入式链表与非侵入式链表

在简单的学习之中, 非侵入式的链表较为常见, 如下

``` C
struct list_example{
	int data;
    struct list_example *next;
}
```

链域与数据域并未进行划分, 或者说, 链域就包含于数据域之中

这样做的话其实也并没有什么坏处, 但毋庸置疑会降低代码复用



侵入式链表则是将链域与数据域区分开来, 单独为链域开辟了一个list结构体, 如下

``` C
struct list_node{
	struct list_node *prev;
    struct list_node *next;
}

typedef struct list_node list_t;

struct list_objectExample{
	//数据域
    int data;
    //链域
    list_t list;
}
```

可见, 将链域与数据域做了区分.



通过这种将链表孤立出来的方式, 我们可以简单地创建新的链表数据结构



## 侵入式链表的具体实现

侵入式链表也有显而易见的缺点, 即:

**对链表进行操作时, 我们无法直接得知内部成员变量的地址偏移量, 因为链表的链域之中仅保存了前后节点的地址**

因此, 要实现正常的数据读取, 增删查改等功能, 我们就要为其进行一系列的地址计算/偏移.

在linux内部, 通过两个宏函数实现了这部分的操作, 如下

``` C
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
```

在本工程中, 也针对这两个宏函数进行了测试.



在这两个宏函数的作用下, 我们就可以通过链域节点的地址, 来获取到整个结构体的头地址及其内部的成员变量地址.



## 侵入式链表的增删查改

在这两个宏函数的辅助下, 侵入式链表的增删查改已与普通链表无异

由于规模较大, 所以具体实现不在此处列出, 具体函数及注释均在list_entry.h中详细列出



当然, 这里实现的链表操作仅为最简版的操作, 不足以与linux内核之中的链表操作相比较, 但已足以实现必要功能

若需进一步研究, 在linux系统中的**/usr/src/include/linux/list.h**之中, 包含了全部的链表操作及注释, 十分详细.



## 侵入式链表的测试实例

在本工程中, main.c之中编写了一部分简单的测试, 如下

``` C
void list_test(){
    struct list_objectExample Head_node;

    //初始化作为头结点
    INIT_LIST_HEAD(&Head_node.list);

    struct list_objectExample node1 = {1, 2, "node1"};
    struct list_objectExample node2 = {3, 4, "node2"};

    //添加两个节点
    list_add(&node1.list, &(Head_node.list));
    list_add(&node2.list, &(Head_node.list));

    struct list_objectExample* struct_ptr;

    //正向遍历节点, 并在每次遍历之后打印,
    //该宏函数的内部用了for循环, 所以可以在后面加其他函数进行执行.
    list_for_each_entry(struct_ptr, &(Head_node.list), list){
        printf("current list string is: %s\r\n", struct_ptr->string);
    };


    //删除node1并打印出链表中全部成员的string变量
    printf("delete node1.\r\n");
    list_del(&(node1.list));

    list_for_each_entry(struct_ptr, &(Head_node.list), list){
        printf("current list string is: %s\r\n", struct_ptr->string);
    };
	
    //将node1通过尾插法插入链表
    printf("add node1 to the tail of list.\r\n");
    list_add_tail(&(node1.list), &(Head_node.list));

    //查找node1节点
    list_for_each_entry(struct_ptr, &(Head_node.list), list){
        if(struct_ptr == &node1){
            printf("node1 has been found.\r\n");
            printf("node1.string: %s\r\n", node1.string);
        }
    };
}
```





### 番外: inline	--	内联函数关键字

inline关键字是向编译器建议 将函数内容嵌入至函数的调用点, 代替正常情况下的函数跳转指令.

``` C
//正常情况:
	函数执行1;
	函数执行2;
	函数跳转;
	跳转函数执行完毕;
	函数执行3;

//内联:
	函数执行1;
	函数执行2;
	跳转函数内容1;
	跳转函数内容2;
	函数执行3;
```

这一操作可以优化程序的性能, **减少函数调用产生的开销.**



### inline关键字的使用场景

1. **小型函数:** inline关键字适合较小的函数, 因为将大型函数嵌入进每个调用点可能导致代码体积急剧增加
2. **频繁调用:** 对于频繁调用的函数, 通过嵌入可以减少函数调用的开销, 提高程序效率.



**注意:**

​	并不是所有inline都会被编译器响应, 编译器有自己的优化方案.

​	不建议过度/滥用inline关键字.
