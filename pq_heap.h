#ifndef __PQ_HEAP__
#define __PQ_HEAP__

#include <stddef.h> /* size_t */

typedef struct pq_heap pq_heap_t;

pq_heap_t *PQHeapCreate(int (*cmp_func_t)(const void *param, 
                                          const void *value));
void PQHeapDestroy(pq_heap_t *pq_heap);

int PQHeapEnqueue(pq_heap_t *pq_heap, void *value);

void *PQHeapDequeue(pq_heap_t *pq_heap);

void *PQHeapPeek(const pq_heap_t *pq_heap);

void PQHeapClear(pq_heap_t *pq_heap);

void *PQHeapErase(pq_heap_t *pq_heap, 
                  int (*is_match)(const void *param, 
                                  const void *value), 
                  void *param);

size_t PQHeapSize(const pq_heap_t *pq_heap);

int PQHeapIsEmpty(const pq_heap_t *pq_heap);

#endif /* __PQ_HEAP__ */
