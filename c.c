#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define HEAP_SIZE 10000

char heap[HEAP_SIZE];

#include <stdbool.h>

#define MAX_DEGREE 3

typedef struct Graph
{
    int status;
    int vertexNo;
    int data;
    int outDegree;
    struct Graph *ptr[MAX_DEGREE];
} Graph;

typedef struct sll_tag
{
    int data;
    struct sll_tag *next;
} sll_node;

typedef struct meta_block
{
    int size_of_memory;
    int free;
    struct meta_block *next;
    int mark;
    int refrence_count;

} meta_data;

meta_data *hdr = (void *)heap;

void Initializie()
{
    hdr->size_of_memory = HEAP_SIZE - sizeof(struct meta_block);
    hdr->free = 1;
    hdr->next = NULL;
    hdr->mark = 0;
    printf("initialized \n");
}

void *alloc(int size_of_data)
{
    meta_data *curr;
    void *result;
    if ((hdr->size_of_memory) == 0)
    {
        Initializie();
    }
    curr = hdr;
    while (((curr->size_of_memory < size_of_data) || (curr->free == 0)) && (curr->next != NULL))
    {
        curr = curr->next;
        // printf("One meta_block checked\n");
    }
    if (curr->size_of_memory == size_of_data)
    {
        curr->free = 0;
        curr->mark = 0;
        curr->refrence_count = 0;
        result = (void *)(++curr);
        printf("free_block found \n");
    }
    else if (curr->size_of_memory > (size_of_data + sizeof(meta_data)))
    {
        meta_data *Next = (void *)(((void *)(curr) + size_of_data + sizeof(meta_data)));
        Next->size_of_memory = curr->size_of_memory - size_of_data - sizeof(meta_data);
        Next->free = 1;
        Next->next = NULL;
        Next->mark = 0;
        Next->refrence_count = 0;
        curr->size_of_memory = size_of_data;
        curr->free = 0;
        curr->mark = 0;
        Next->refrence_count = 0;
        curr->next = Next;
        result = (void *)(++curr);
        printf("free block found \n");
    }
    else
    {
        result = NULL;
        printf("out of memory\n");
    }
    return result;
}

void merge()
{
    meta_data *curr, *Next;
    int flag = 0;
    curr = hdr;
    Next = curr->next;
    while (flag == 0 && Next != NULL)
    {
        if (curr->free == 1 && Next->free == 1)
        {
            if (Next->next != NULL && Next->next->free == 1)
            {
                curr->size_of_memory = curr->size_of_memory + Next->size_of_memory + Next->next->size_of_memory + 2 * sizeof(meta_data);
                curr->next = Next->next->next;
            }
            else
            {
                curr->size_of_memory = curr->size_of_memory + Next->size_of_memory + sizeof(meta_data);
                curr->next = Next->next;
            }
            flag = 1;
        }
        else
        {
            curr = curr->next;
            Next = Next->next;
        }
    }
}

void freee(void *ptr)
{
    if (((void *)heap <= ptr) && (ptr <= (void *)(heap + HEAP_SIZE)))
    {
        meta_data *curr = (meta_data *)ptr;
        --curr;
        curr->free = 1;
        curr->mark = 0;
        merge();
        printf("memroy freeed \n");
    }
    else
    {
        printf("freee called on unallocated memory\n ");
    }
}

void showHeap()
{
    meta_data *ptr;
    ptr = hdr;
    while (ptr != NULL)
    {
        if (ptr->free == 0)
        {
            printf("%12s\n", "ALLOCATED");
            printf("----------------------\n");
            printf("%10s %7d\n", "Size: ", ptr->size_of_memory);
            printf("----------------------\n");
        }
        else
        {
            printf("%12s\n", "FREE");
            printf("----------------------\n");
            printf("%10s %7d\n", "Size: ", ptr->size_of_memory);
            printf("----------------------\n");
        }
        ptr = ptr->next;
    }
}

static int Vertices = 0;

Graph *createGraphNode(int val, int ptrs)
{
    Graph *newNode = (Graph *)alloc(sizeof(Graph));
    newNode->vertexNo = (Vertices)++;
    for (int i = 0; i < MAX_DEGREE; i++)
        newNode->ptr[i] = NULL;
    newNode->data = val;
    newNode->outDegree = ptrs;
    newNode->status = 0;
    return newNode;
}

void create_adjacent_matrix(int adj[][8])
{
    int i, j;
    meta_data *temp = hdr;
    Graph *gtemp1;
    while (temp != NULL)
    {
        // printf("1");
        if (temp->free != 1)
        {
            gtemp1 = (void *)temp + sizeof(meta_data);
            i = gtemp1->vertexNo;
            for (int k = 0; k < gtemp1->outDegree; k++)
            {
                j = gtemp1->ptr[k]->vertexNo;
                adj[i][j] = 1;
            }
        }
        temp = temp->next;
    }
}
void initialize_adjacent_matrix(int adj[][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            adj[i][j] = 0;
        }
    }
}
void print_adjacent_matrix(int adj[][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%d\t", adj[i][j]);
        }
        printf("\n");
    }
}

void mark(Graph *root)
{
    if (root != NULL)
    {
        meta_data *temp;
        int i;
        temp = (void *)root - sizeof(meta_data);
        temp->mark = 1;
        // printf("1\n");

        for (i = 0; i < root->outDegree; i++)
        {
            mark(root->ptr[i]);
        }
    }
}

void sweap()
{
    meta_data *temp = hdr;
    while (temp != NULL)
    {
        if (temp->mark == 0)
        {
            // printf("0\n");
            freee((void *)temp + sizeof(meta_data));
        }
        temp = temp->next;
    }
}

void refrence_count(Graph *root)
{
    if (root != NULL)
    {
        meta_data *temp;
        int i;
        temp = (void *)root - sizeof(meta_data);
        temp->refrence_count = temp->refrence_count + 1;
        // printf("1\n");

        for (i = 0; i < root->outDegree; i++)
        {
            refrence_count(root->ptr[i]);
        }
    }
}
void create_adjacent_matrix_ref_count(int adj[][8])
{
    int i, j;
    meta_data *temp = hdr;
    Graph *gtemp1;
    while (temp != NULL)
    {
        // printf("1");
        if (temp->refrence_count != 0)
        {
            gtemp1 = (void *)temp + sizeof(meta_data);
            i = gtemp1->vertexNo;
            for (int k = 0; k < gtemp1->outDegree; k++)
            {
                j = gtemp1->ptr[k]->vertexNo;
                adj[i][j] = 1;
            }
        }
        temp = temp->next;
    }
}

int main()
{
    int adjoint[8][8];
    initialize_adjacent_matrix(adjoint);

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
    
    V1=NULL;
    V2=NULL;
    V3=NULL;
    V4=NULL;
    V5=NULL;
    V6=NULL;
    V7=NULL;
    V8=NULL;

    showHeap();
    printf("\n \n");
    create_adjacent_matrix(adjoint);
    print_adjacent_matrix(adjoint);

    mark(Root1);
    mark(Root2);
    sweap();
    printf("adjacent_matrix after mark sweap\n ");
    printf("-------------------------------------------------------------\n");
    initialize_adjacent_matrix(adjoint);
    create_adjacent_matrix(adjoint);
    print_adjacent_matrix(adjoint);

    showHeap();
    printf("\n \n");
    printf("adjacent_matrix after refrence_count\n ");
    printf("-------------------------------------------------------------\n");
    refrence_count(Root1);
    refrence_count(Root2);
    initialize_adjacent_matrix(adjoint);
    create_adjacent_matrix_ref_count(adjoint);
    print_adjacent_matrix(adjoint);

    showHeap();

    printf("%d", sizeof(meta_data));
    return 0;
}