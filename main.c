#include <stdio.h>
#include "List_Entry.h"

void offsetof_test(void){
    struct list_objectExample test;
    int len;

    printf("struct len = %d\r\n", sizeof(struct list_objectExample));

    len = offsetof(struct list_objectExample, data1);
    printf("data1 offset = %d\r\n", len);

    len = offsetof(struct list_objectExample, data2);
    printf("data2 offset = %d\r\n", len);

    len = offsetof(struct list_objectExample, string);
    printf("string offset = %d\r\n", len);
}

void container_test(void){
    struct list_objectExample container_test_init= { .data1 = 12, .data2=66, .string = "list_entry"};

    uint8_t *char_ptr = &container_test_init.data1;

    struct list_objectExample* test_object = container_of(char_ptr, struct list_objectExample, data1);

    printf("init test struct address is: %p\r\n", &container_test_init);
    printf("get test struct address is: %p\r\n", test_object);

    printf("test_struct->data1 = %d\r\n"
           "test_struct->data2 = %d\r\n"
           "test_struct->string = %s\r\n"
           ,test_object->data1, test_object->data2, test_object->string);
}

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


    printf("delete node1.\r\n");
    list_del(&(node1.list));

    list_for_each_entry(struct_ptr, &(Head_node.list), list){
        printf("current list string is: %s\r\n", struct_ptr->string);
    };

    printf("add node1 to the tail of list.\r\n");
    list_add_tail(&(node1.list), &(Head_node.list));

    list_for_each_entry(struct_ptr, &(Head_node.list), list){
        if(struct_ptr == &node1){
            printf("node1 has been found.\r\n");
            printf("node1.string: %s\r\n", node1.string);
        }
    };
}

int main() {
    printf("Hello, World!\n");

//    offsetof_test();

//    container_test();

    list_test();

    return 0;
}
