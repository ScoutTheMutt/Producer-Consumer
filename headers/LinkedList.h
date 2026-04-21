#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
} Node;

Node* createNode(int data);
void insertSortedNode(Node** head, int data);
void deleteNode(Node** head, int key);
char* listToString(const Node* head);