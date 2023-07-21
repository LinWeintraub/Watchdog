/* ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
 ~ Author        Lin Weintraub                                   ~
 * Version       1.0.0                                           *
 ~ Date          1/12/22                                         ~
 * Description   Heap                                            *
 * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ */
 
#include <stdlib.h> /*malloc*/
#include <assert.h> /*assert*/
#include <string.h> /*memcpy*/

#ifndef NDBUG
#include <stdio.h> /*printf*/
#endif

#include "heap.h"
#include "vector.h"

#define HEAP_SIZE 255 /*2^8 - 1*/

struct heap
{
    vector_t *vector;
    cmp_func_t cmp_func_p;
};

/*---------------------------Function Declarations----------------------------*/
static int GetParentIndex(int index);
static int GetLeftChildIndex(int index);
static int GetRightChildIndex(int index);
static void Swap(void *x, void *y);
heap_t *HeapifyUp(heap_t *heap, int index);
void HeapifyDown(heap_t *heap, int index);

/*---------------------------------API-Functions------------------------------*/
heap_t *HeapCreate(cmp_func_t cmp_func_p)
{
    heap_t *heap = malloc(sizeof(heap_t));
    vector_t *vector = NULL;
    if (NULL == heap)
	{   
		return NULL;
	}

    vector = VectorCreate(HEAP_SIZE, sizeof(void *));
    if (NULL == vector)
	{   
        free(heap); heap = NULL;
		return NULL;
	}

    heap->vector = vector;
    heap->cmp_func_p = cmp_func_p;

    return heap;
}
/*----------------------------------------------------------------------------*/
void HeapDestroy(heap_t *heap)
{
    VectorDestroy(heap->vector);
    free(heap); heap = NULL;
}
/*----------------------------------------------------------------------------*/
status_t HeapPush(heap_t *heap, void *value)
{
    int status = 0;
    size_t index = 0;

    assert(heap);

    status = VectorAppend(heap->vector, &value);
    if (SUCCESS != status)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    index = VectorSize(heap->vector) - 1;

    HeapifyUp(heap, index);
        
    return SUCCESS;
}
/*----------------------------------------------------------------------------*/
void *HeapPop(heap_t *heap)
{
    void *last_element = NULL;
    void *pop_element = NULL;
    size_t last_index = 0;
    void *save_pop_element = NULL;

    assert(heap);

    last_index = VectorSize(heap->vector) - 1;
    last_element = VectorGetAccessToElement(heap->vector, last_index);
    pop_element = VectorGetAccessToElement(heap->vector, 0);

    Swap(pop_element, last_element);
    save_pop_element = *(void **)VectorGetAccessToElement(heap->vector, last_index);
    VectorRemoveLast(heap->vector);
    HeapifyDown(heap, 0);

    return save_pop_element;
}
/*----------------------------------------------------------------------------*/
void *HeapPeek(const heap_t *heap)
{
    return *(void**)(VectorGetAccessToElement(heap->vector, 0));
}
/*----------------------------------------------------------------------------*/
size_t HeapSize(const heap_t *heap)
{
    return (VectorSize(heap->vector));
}
/*----------------------------------------------------------------------------*/
int HeapIsEmpty(const heap_t *heap)
{
    return (VectorIsEmpty(heap->vector));
}
/*----------------------------------------------------------------------------*/
void *HeapRemove(heap_t *heap,
                    int (*is_match)(const void *param, const void *value),
                    const void *param)
{
    int status = 0;
    size_t i = 0;
    size_t index = 0;
    void *index_element = NULL;
    void *last_element = NULL;    
    void *save_remove_element = NULL;
    size_t last_index = 0;

    assert(heap);

    last_index = VectorSize(heap->vector) - 1;
    last_element = VectorGetAccessToElement(heap->vector, last_index);

    for (i = 0; i < VectorSize(heap->vector); ++i)
    {
        index_element = VectorGetAccessToElement(heap->vector, i);
        status = is_match(param, *(void **)index_element);

        if (1 == status)
        {
            index = i;
            index_element = VectorGetAccessToElement(heap->vector, index);    
            Swap(index_element, last_element);
            save_remove_element = *(void **)VectorGetAccessToElement(heap->vector, last_index);
            VectorRemoveLast(heap->vector);
            HeapifyDown(heap, index);
            break;
        }
    }

    return (status == 0 ? NULL : save_remove_element);
}


/*--------------------------------Static-Functions----------------------------*/
static int GetParentIndex(int index) 
{
    return ((index - 1) / 2);
}

static int GetLeftChildIndex(int index) 
{
    return (2 * index + 1);
}

static int GetRightChildIndex(int index) 
{
    return (2 * index + 2);
}

static void Swap(void *ptr_1, void *ptr_2)
{
	char temp = 0;
	char *ch_ptr1 = NULL;
	char *ch_ptr2 = NULL;
    size_t element_size = sizeof(void *);

    assert(ptr_1);
    assert(ptr_2);

	ch_ptr1 = (char *)ptr_1;
	ch_ptr2 = (char *)ptr_2;
	
	while (0 != element_size)
	{
		memcpy(&temp, ch_ptr1, 1);
		*ch_ptr1 = *ch_ptr2;
		*ch_ptr2 = temp;
		++ch_ptr1;
		++ch_ptr2;
		--element_size;
	}
}

heap_t *HeapifyUp(heap_t *heap, int index) 
{
    int cmp_status = 0;
    int parent = 0; 
    void *index_element = NULL;
    void *parent_element = NULL;

    assert(heap);

    if (0 < index)
    {    
        parent = GetParentIndex(index); 
        index_element = VectorGetAccessToElement(heap->vector, index);
        parent_element = VectorGetAccessToElement(heap->vector, parent);
        
        cmp_status = heap->cmp_func_p(*(void **)index_element, *(void **)parent_element);

        if (0 < cmp_status)
        {
            Swap(index_element, parent_element);
        }

        HeapifyUp(heap, parent);
    }
    
    return heap;
}

void HeapifyDown(heap_t *heap, int index) 
{
    int left_child = 0;
    int right_child = 0;

    int cmp_status_left = 0;
    int cmp_status_right = 0;
    
    void *index_element = NULL;
    void *left_element = NULL;
    void *right_element = NULL;
    void *heapify_element = NULL;

    int heapify_index = index;
    size_t size = 0;
    
    assert(heap);

    size = VectorSize(heap->vector);

    left_child = GetLeftChildIndex(index);
    right_child = GetRightChildIndex(index);

    if ((size_t)heapify_index == size)
    {
        return;
    }

    index_element = VectorGetAccessToElement(heap->vector, index);
    heapify_element = VectorGetAccessToElement(heap->vector, heapify_index);

    if ((size_t)left_child < size)
    {
        left_element = VectorGetAccessToElement(heap->vector, left_child);

        cmp_status_left = heap->cmp_func_p(*(void **)heapify_element, *(void **)left_element);

        if (0 > cmp_status_left)
        {
            heapify_index = left_child;
            heapify_element = VectorGetAccessToElement(heap->vector, heapify_index);
        }
    }

    if ((size_t)right_child < size)
    {
        right_element = VectorGetAccessToElement(heap->vector, right_child);
        cmp_status_right = heap->cmp_func_p(*(void **)heapify_element, *(void **)right_element);

        if (0 > cmp_status_right)
        {
            heapify_index = right_child;
        }
    }

    if (heapify_index != index)
    {
        heapify_element = VectorGetAccessToElement(heap->vector, heapify_index);
        Swap(index_element, heapify_element);
        HeapifyDown(heap, heapify_index);
    }
}
    
#ifndef NDBUG
void PrintHeap(heap_t *heap)
{
    size_t size = 0;
    size_t i = 0;
    void *data = NULL;

    assert(heap);

    size = VectorSize(heap->vector);

    for(i = 0; i < size; i++)
    {
        data = *(int **)VectorGetAccessToElement(heap->vector, i);
        printf("%d-->", *(int *)data);
    }
    printf("\n");
}
#endif
