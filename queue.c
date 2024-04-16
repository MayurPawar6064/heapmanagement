#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

#define ERROR_VALUE NULL

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

bool isEmpty(Queue** queue) {
    return (*queue)->front == NULL;
}

bool enqueue(Queue** queue, Graph* value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        return false;
    }
    newNode->data = value;
    newNode->next = NULL;
    if (isEmpty(queue)) {
        (*queue)->front = newNode;
        (*queue)->rear = newNode;
    } else {
        (*queue)->rear->next = newNode;
        (*queue)->rear = newNode;
    }
    return true;
}

Graph* dequeue(Queue** queue, bool* success) {
    if (isEmpty(queue)) {
        *success = false;
        printf("Queue underflow!\n");
        return ERROR_VALUE;
    }
    Node* temp = (*queue)->front;
    Graph* dequeuedItem = temp->data;
    (*queue)->front = (*queue)->front->next;
    free(temp);
    *success = true;
    return dequeuedItem;
}

Graph* peek(Queue** queue, bool* success) {
    if (isEmpty(queue)) {
        *success = false;
        printf("Queue is empty!\n");
        return ERROR_VALUE;
    }
    *success = true;
    return (*queue)->front->data;
}

void display(Queue** queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty!\n");
        return;
    }
    Node* current = (*queue)->front;
    printf("Queue: ");
    while (current != NULL) {
        printf("%d ", current->data->data);
        current = current->next;
    }
    printf("\n");
}

void destroyQueue(Queue** queue) {
    Node* current = (*queue)->front;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    free(*queue);
    *queue = NULL;
}
