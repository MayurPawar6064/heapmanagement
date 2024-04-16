#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#define MAX_DEGREE 3

typedef struct Graph {
    int status;
    int vertexNo;
    int data;
    int outDegree;
    struct Graph* ptr[MAX_DEGREE];
} Graph;

typedef struct Node {
    Graph* data;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

Queue* createQueue();
bool isEmpty(Queue** queue);
bool enqueue(Queue** queue, Graph* value);
Graph* dequeue(Queue** queue, bool* success);
Graph* peek(Queue** queue, bool* success);
void display(Queue** queue);
void destroyQueue(Queue** queue);

#endif
