/* ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
 ~ Author        Lin Weintraub                                   ~
 * Version       1.0.0                                           *
 ~ Date          1/12/22                                         ~
 * Description   PQ Heap                                         *
 * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ */
 
#include <assert.h> /*assert*/
#include <stdlib.h> /*malloc, free*/

#include "pq_heap.h"
#include "heap.h"

struct pq_heap
{
	heap_t *heap;
};

/*---------------------------------API-Functions------------------------------*/
pq_heap_t *PQHeapCreate(int (*cmp_func_t)(const void *param, 
                                          const void *value))
{
	pq_heap_t *pq_heap = malloc(sizeof(pq_heap_t));
	heap_t *heap = NULL;
    if (NULL == pq_heap)
	{   
		return NULL;
	}
	
	assert(cmp_func_t);
	
	heap = HeapCreate(cmp_func_t);
    if (NULL == heap)
	{   
		return NULL;
	}

	pq_heap->heap = heap;
	
	return pq_heap;
}
/*----------------------------------------------------------------------------*/
void PQHeapDestroy(pq_heap_t *pq_heap)
{
	assert(pq_heap);
	
	HeapDestroy(pq_heap->heap); pq_heap->heap = NULL;
	free(pq_heap); pq_heap = NULL;
}
/*----------------------------------------------------------------------------*/
int PQHeapEnqueue(pq_heap_t *pq_heap, void *value)
{
	int status = SUCCESS;

	assert(pq_heap);
	assert(value);

	status = HeapPush(pq_heap->heap, value);
	if (SUCCESS != status)
	{
		status = FAILURE;
	}
	
	return status;
}
/*----------------------------------------------------------------------------*/
void *PQHeapDequeue(pq_heap_t *pq_heap)
{
	void *pop_element = NULL;

	assert(pq_heap);
	assert(0 != HeapSize(pq_heap->heap)); 

	pop_element = HeapPeek(pq_heap->heap);
	HeapPop(pq_heap->heap);

	return(pop_element);
}
/*----------------------------------------------------------------------------*/
void *PQHeapPeek(const pq_heap_t *pq_heap)
{
	assert(pq_heap);
	assert(0 != HeapSize(pq_heap->heap)); 
	
	return(HeapPeek(pq_heap->heap));

}
/*----------------------------------------------------------------------------*/
void PQHeapClear(pq_heap_t *pq_heap)
{
	assert(pq_heap);

	while(!HeapIsEmpty(pq_heap->heap))
	{
		HeapPop(pq_heap->heap);
	}
}
/*----------------------------------------------------------------------------*/
void *PQHeapErase(pq_heap_t *pq_heap, 
                  int (*is_match)(const void *param, 
                                  const void *value), 
                  void *param)
{
	return(HeapRemove(pq_heap->heap, is_match, param));
}
/*----------------------------------------------------------------------------*/
size_t PQHeapSize(const pq_heap_t *pq_heap)
{
	assert(pq_heap);
	
	return (HeapSize(pq_heap->heap));
}
/*----------------------------------------------------------------------------*/
int PQHeapIsEmpty(const pq_heap_t *pq_heap)
{
	assert(pq_heap);
	
	return (HeapIsEmpty(pq_heap->heap));
}
