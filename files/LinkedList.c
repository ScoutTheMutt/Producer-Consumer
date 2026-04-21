#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../headers/LinkedList.h"

Node* createNode(int data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void insertSortedNode(Node** head, int data) {
    if (data > 100 || data < 1) {
        printf("Value %d out of allowed range (1-100). Not inserting.\n", data);
        return;
    }
    
    Node* newNode = createNode(data);

    if (*head == NULL || data <= (*head)->data) {
        newNode->next = *head;
        if (*head != NULL) {
            (*head)->prev = newNode;
        }
        *head = newNode;
        printf("\nInserted node at head with data %d\n", data);
        return;
    }
    Node* current = *head;

    while (current->next != NULL && current->next->data < data) {
    current = current->next;
    }

    newNode->next = current->next;
    newNode->prev = current;
    if (current->next != NULL) {
        current->next->prev = newNode;
    }
    current->next = newNode;
    printf("Inserted node with data %d\n", data);
}

void deleteNode(Node** head, int key) {
    Node* temp = *head;
    Node* prev = NULL;

    if (temp == NULL) {
        return;
    }

    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }

    if (prev == NULL) {
        *head = temp->next;
    } else {
        prev->next = temp->next;
        }

    printf("Deleted node with key %d\n", key);
    free(temp);
}

char* listToString(const Node* head) {
    if (head == NULL) {
        char* s = (char*)malloc(1);
        if (s) s[0] = '\0';
        return s;
    }

    // runs through the list to compute the length so we know what buffer size we will need.
    size_t len = 0;
    const Node* cur = head;
    int first = 1;
    while (cur) {
        if (!first) len += 1; // comma
        len += sizeof(cur->data);
        first = 0;
        cur = cur->next;
    }

    // Allocate buffer
    char* out = (char*)malloc(len + 1); // +1 for c string terminator
    if (!out) return NULL;


    // Goes back through and writes the values one by one.
    char* p = out;
    cur = head;
    first = 1;
    while (cur) {
        if (!first) { *p++ = ','; }
        int written = sprintf(p, "%d", cur->data);
        p += written;
        first = 0;
        cur = cur->next;
    }
    // Null terminator
    *p = '\0';
    return out;
}