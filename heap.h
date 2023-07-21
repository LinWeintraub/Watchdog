#ifndef __HEAP__
#define __HEAP__

#include <stddef.h>

typedef int (*cmp_func_t)(const void *param, const void *value);

typedef enum
{
    SUCCESS = 0,
    FAILURE = 1,
    MEMORY_ALLOCATION_ERROR = 2
} status_t;

typedef struct heap heap_t; 



/*--------------------------------------------------------
Params: cmp_func_p - comparison function used to determine
        the order of elements.
        element_size - size of each element in the heap.
Return: Pointer to a heap.
        If memory couldn't be allocated, NULL is returned.
Complexity: O(1) (depends on malloc) */

heap_t *HeapCreate(cmp_func_t cmp_func_p);

/*--------------------------------------------------------
Params: heap - pointer to a heap.
Return: -
Complexity: O(1) */
void HeapDestroy(heap_t *heap);

/*--------------------------------------------------------
Params: Inserts a new element into the array.
Return: Success status.
Complexity: O(logn) */

status_t HeapPush(heap_t *heap, void *value);


/*--------------------------------------------------------
Params: Removes the maximum element in the heap.
Return: The value of the maximum element.
Complexity: O(logn) */

void *HeapPop(heap_t *heap);


/*--------------------------------------------------------
Params: heap_t - heap 
Return: top element of heap, max value in heap.
Complexity: O(1) */
void *HeapPeek(const heap_t *heap);


/*--------------------------------------------------------
Params: heap_t - heap 
Return: amount of elements in heap.
Complexity: O(1) since a vector is used for implementation*/
size_t HeapSize(const heap_t *heap);


/*--------------------------------------------------------
Params: heap_t - heap 
Return: 1 if heap is empty, else 0.
Complexity: O(1) */
int HeapIsEmpty(const heap_t *heap);

/*--------------------------------------------------------
Params: heap_t - heap, value - value to remove from heap.
Return: void *, the element that was removed.
        NULL if value wasn't found. 
Complexity: O(n) */
void *HeapRemove(heap_t *heap, int (*is_match)(const void *param, const void *value), const void *param);


#endif /* __HEAP__ */
