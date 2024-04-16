#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"
#define MAX_DEGREE 3
#define MAX_HEAP_SIZE 262152 // size of 256 kilobytes + threshold
#define MARKED 1
#define UNMARKED -1
typedef enum
{
    NTINIT,
    INIT
} status;
typedef enum
{
    FALSE,
    TRUE
} boolean;

static status sc = NTINIT;
typedef struct heapmetadata
{
    size_t size;
    struct heapmetadata *next;
} MetaData;

void print(MetaData *ptr)
{
    printf("\n -> SIZE OF THE BLOCK = %d\n ", ptr->size - 8);
}

static int Vertices = 0;

typedef struct List
{
    Graph *node;
    struct List *next;
} List;

typedef struct Root
{
    Graph *node;
} rootPtr;

void DisplayLists(MetaData *head)
{ // this has been made just to verify whether all allocation and deallocation calls are being done correctly or not
    if (head == NULL)
    {
        printf("\nNULL\n");
    }
    else
    {
        MetaData *ptr = head;
        while (ptr)
        {
            print(ptr);
            ptr = ptr->next;
        }
    }
}

#define threshold 8
char simulatedHeap[MAX_HEAP_SIZE];
MetaData *freelistHead = (void *)simulatedHeap;

void initializeHead()
{
    freelistHead->size = MAX_HEAP_SIZE - threshold;
    freelistHead->next = NULL;
    // printf("\n----Heap simulation Successfull----\n");
}

MetaData *allocatedhead = NULL;

void insertAllocated(MetaData *block)
{
    if (allocatedhead == NULL)
    {
        allocatedhead = block;
        block->next = NULL;
    }
    else
    {
        MetaData *itr = allocatedhead;
        while (itr->next)
        {
            itr = itr->next;
        }
        itr->next = block;
        block->next = NULL;
    }
}

void *Allocate(size_t sizerequest)
{
    if (sc == NTINIT)
    {
        initializeHead();
        sc = INIT;
    }
    if (sizerequest == 0)
    {
        printf("\nCouldn't Allocate Block of zero size \n");
        printf("\nAllocate() Failed\n");
        return NULL;
    }
    sizerequest = (sizerequest < threshold) ? threshold : sizerequest;
    void *ret_val = NULL;
    MetaData *allocatedblock;
    sizerequest += threshold;
    MetaData *itr = freelistHead;
    MetaData *prev = NULL;
    boolean found = FALSE;
    while (itr && !found)
    {
        if (itr->size >= sizerequest)
        {
            found = TRUE;
        }
        else
        {
            prev = itr;
            itr = itr->next;
        }
    }
    if (itr == NULL)
    {
        ret_val = NULL;
        printf("\nUnable to allocate Memory \n");
        printf("Memory is full (or) External Fragmentation\n\n");
        return ret_val;
    }
    else if (itr && (itr->size - sizerequest <= threshold))
    {
        allocatedblock = itr;
        if (prev)
            prev = prev->next;
        else
            freelistHead = NULL;
    }
    else if (itr && (itr->size - sizerequest > threshold))
    {
        allocatedblock = itr;
        void *newhead = (void *)((void *)itr + sizerequest);
        MetaData *next = itr->next;
        int prevsize = itr->size;
        itr = newhead;
        itr->size = prevsize - sizerequest;
        itr->next = next;
        allocatedblock->size = sizerequest - threshold;
        if (prev == NULL)
            freelistHead = itr;
        else
            prev->next = itr;
    }
    insertAllocated(allocatedblock);
    ret_val = (void *)(allocatedblock + 1);
    return ret_val;
}

void Deallocate(void *p)
{
    if (p == NULL)
    {
        printf("\nProvide a valid pointer to Deallocate\n");
        printf("\nDeallocation failed");
        return;
    }
    void *startaddress = p - threshold;
    MetaData *itr = allocatedhead;
    MetaData *ptr = NULL;
    while (itr && (void *)itr != startaddress)
    {
        ptr = itr;
        itr = itr->next;
    }
    if (itr)
    {
        size_t size = itr->size;
        void *endaddress = startaddress + size + threshold;

        // removing the required deallocated block from allocated list
        if (ptr == NULL)
        {
            allocatedhead = itr->next;
        }
        else
        {
            ptr->next = itr->next;
        }
        itr->next = NULL;
        // done removing

        MetaData *freeitr = freelistHead;
        MetaData *freeprev = NULL;
        while (endaddress > (void *)freeitr)
        {
            freeprev = freeitr;
            freeitr = freeitr->next;
        }
        void *prevend = NULL;
        if (!freeprev && freeitr)
        {
            if (endaddress == (void *)freeitr)
            {
                int temp = freeitr->size;
                MetaData *next = freeitr->next;
                freelistHead = (MetaData *)startaddress;
                freelistHead->size = temp + threshold + size;
                freelistHead->next = next;
            }
            else
            {
                itr->next = freelistHead;
                freelistHead = itr;
            }
        }
        else if (freeprev && !freeitr)
        {
            void *add = (void *)freeprev + freeprev->size + threshold;
            if (add == startaddress)
            {
                freeprev->size += size + threshold;
            }
            else
            {
                freeprev->next = itr;
                itr->next = NULL;
            }
        }
        else
        {
            void *left = (void *)freeprev + freeprev->size + threshold;
            if (startaddress != left && endaddress != freeitr)
            {
                itr->next = freeitr;
                freeprev->next = itr;
            }
            else if (startaddress == left && endaddress != freeitr)
            {
                freeprev->size += itr->size + threshold;
            }
            else if (startaddress != left && endaddress == freeitr)
            {
                MetaData *next = freeitr->next;
                int t = freeitr->size;
                freeitr = (MetaData *)endaddress;
                freeitr->size = t + size + threshold;
                freeitr->next = next;
            }
            else
            {
                freeprev->size += itr->size + freeitr->size + (2 * threshold);
                freeprev->next = freeitr->next;
                freeitr->next = NULL;
            }
        }
        printf("\n Memory Block Of [ SIZE = %d ]freed Up\n", itr->size - threshold);
    }
    else
    {
        printf("\nFree up a valid element from a heap \n");
        printf("\n Deallocate(void *p) failed \n");
    }
}

List *allocatedList = NULL;
List *rootPtrs = NULL;
void insertInGraphList(List **root, Graph *newNode)
{
    List *Node = (List *)malloc(sizeof(List));
    Node->node = newNode;
    Node->next = NULL;
    if (*root == NULL)
    {
        *root = Node;
    }
    else
    {
        List *ptr = *root;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = Node;
    }
}
Graph *createGraphNode(int val, int ptrs)
{
    Graph *newNode = (Graph *)Allocate(sizeof(Graph));
    newNode->vertexNo = (Vertices)++;
    for (int i = 0; i < MAX_DEGREE; i++)
        newNode->ptr[i] = NULL;
    newNode->data = val;
    newNode->outDegree = ptrs;
    newNode->status = 0;
    insertInGraphList(&allocatedList, newNode);
    return newNode;
}

void createAdjacencyMatrix(Graph *root1, int adj[][8], int vertices)
{
    Queue *q = createQueue();
    enqueue(&q, root1);
    int visited[vertices];
    for (int i = 0; i < vertices; i++)
    {
        visited[i] = 0;
    }
    visited[root1->vertexNo] = 1;
    adj[root1->vertexNo][root1->vertexNo] = 1;
    bool status = true;
    while (!isEmpty(&q))
    {
        Graph *node = dequeue(&q, &status);
        for (int i = 0; i < node->outDegree; i++)
        {
            if ((node->ptr[i] != NULL) && (!visited[node->ptr[i]->vertexNo]))
            {
                enqueue(&q, node->ptr[i]);
                visited[node->ptr[i]->vertexNo] = 1;
                adj[node->vertexNo][node->ptr[i]->vertexNo] = 1;
            }
        }
    }
}

void printAdjacencyMatrix(int matrix[][8], int vertices)
{
    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < vertices; j++)
        {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void referenceCountMechanism(int adj[][8], int allocated)
{
    int refCounts[allocated];
    for (int i = 0; i < allocated; i++)
    {
        refCounts[i] = 0;
    }
    for (int i = 0; i < allocated; i++)
    {
        for (int j = 0; j < allocated; j++)
        {
            if (adj[i][j] == 1)
            {
                refCounts[j]++;
            }
        }
    }
    int vertex = 0;
    List *ptr = allocatedList;
    while (ptr != NULL)
    {
        ptr->node->status = refCounts[vertex++];
        ptr = ptr->next;
    }
    for (int i = 0; i < allocated; i++)
    {
        printf("%d\t", refCounts[i]);
    }
    printf("\n\n");
    ptr = allocatedList;
    List *prev = NULL;
    // GARBAGE COLLECTION
    while (ptr != NULL)
    {
        List *next = ptr->next;
        if (ptr->node->status == 0)
        {
            if (prev == NULL)
            {
                allocatedList = ptr->next;
            }
            else
            {
                prev->next = ptr->next;
            }
            Deallocate(ptr->node);
            ptr = next;
        }
        else
        {
            prev = ptr;
            ptr = next;
        }
    }
    printf("\n SIZE OF ALLOCATED BOLCKS IN THE HEAP : \n");
    DisplayLists(allocatedhead);
    printf("\n SIZE OF FREE AND MERGED BOLCKS IN THE HEAP : \n");
    DisplayLists(freelistHead);
}

void markAndSweep(int adj[][8], int allocated)
{
    // MARK -- PHASE
    List *ptr = allocatedList;
    while (ptr != NULL)
    {
        ptr->node->status = UNMARKED;
        ptr = ptr->next;
    }
    int marked[allocated];
    for (int i = 0; i < allocated; i++)
    {
        marked[i] = UNMARKED;
    }
    for (int i = 0; i < allocated; i++)
    {
        for (int j = 0; j < allocated; j++)
        {
            if (adj[i][j] == 1)
            {
                marked[j] = MARKED;
            }
        }
    }
    int vertex = 0;
    ptr = allocatedList;
    while (ptr != NULL)
    {
        ptr->node->status = marked[vertex++];
        ptr = ptr->next;
    }

    // SWEEP -- PHASE
    ptr = allocatedList;
    List *prev = NULL;

    while (ptr != NULL)
    {
        List *next = ptr->next;
        if (ptr->node->status == UNMARKED)
        {
            if (prev == NULL)
            {
                allocatedList = ptr->next;
            }
            else
            {
                prev->next = ptr->next;
            }
            Deallocate(ptr->node);
            ptr = next;
        }
        else
        {
            prev = ptr;
            ptr = next;
        }
    }
    printf("\n SIZE OF ALLOCATED BOLCKS IN THE HEAP : \n");
    DisplayLists(allocatedhead);
    printf("\n SIZE OF FREE AND MERGED BOLCKS IN THE HEAP : \n");
    DisplayLists(freelistHead);
}
int main()
{
    int vertices = 8;
    Graph *V1 = createGraphNode(1, 3);
    Graph *V2 = createGraphNode(2, 0);
    Graph *V3 = createGraphNode(3, 2);
    Graph *V4 = createGraphNode(5, 1);
    Graph *V5 = createGraphNode(7, 2);
    Graph *V6 = createGraphNode(8, 1);
    Graph *V7 = createGraphNode(9, 0);
    Graph *V8 = createGraphNode(10, 0);

    V1->ptr[0] = V2;
    V1->ptr[1] = V7;
    V1->ptr[2] = V8;
    V3->ptr[0] = V6;
    V3->ptr[1] = V8;
    V4->ptr[0] = V1;
    V5->ptr[0] = V1;
    V5->ptr[1] = V6;
    V6->ptr[0] = V7;

    Graph *Root1 = V4;
    Graph *Root2 = V1;

    int adjRoot1[8][8];
    int adjRoot2[8][8];
    int adjacency[8][8];

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            adjRoot1[i][j] = 0;
            adjRoot2[i][j] = 0;
        }
    }

    createAdjacencyMatrix(Root1, adjRoot1, vertices);
    createAdjacencyMatrix(Root2, adjRoot2, vertices);

    printAdjacencyMatrix(adjRoot1, vertices);
    printAdjacencyMatrix(adjRoot2, vertices);

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (adjRoot1[i][j] == 1 || adjRoot2[i][j] == 1)
            {
                adjacency[i][j] = 1;
            }
            else
            {
                adjacency[i][j] = 0;
            }
        }
    }

    printAdjacencyMatrix(adjacency, vertices);
    referenceCountMechanism(adjacency, vertices);
    printf("\n\n");
    List *ptr = allocatedList;
    while (ptr)
    {
        printf("[%d] - [%d] - [%d] \n", ptr->node->vertexNo, ptr->node->data, ptr->node->status);
        ptr = ptr->next;
    }

    return 0;

    // gcc heapmanagment.c queue.c -o finale.exe
    // ./finale.exe
}