//
// Created by Yiping Deng on 4/18/20.
//

#ifndef DOTS_CLIENT_LINK_LIST_H
#define DOTS_CLIENT_LINK_LIST_H

typedef struct list_node {
    char* val;
    struct list_node* next;
} list_node;

inline static list_node* list_add(list_node* tail, char* val) {
    list_node* node = malloc(sizeof(list_node));
    node->next = tail;
    char* buf = malloc(strlen(val) + 1);
    strcpy(buf, val);
    node->val = buf;
    return node;
}

#endif //DOTS_CLIENT_LINK_LIST_H
