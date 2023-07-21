#ifndef __VECTOR__
#define __VECTOR__

/* 
A dynamic array allocates memory dynamically in order to be able to add
or remove elements and at the same time have random access to our
stored elements. 
The Growth Factor is 2.
When functions Shrink, Reserve & Append fails they return 1.
When functions Create & GetAccessToElement fails they return NULL.
*/

#include <stddef.h>

typedef struct vector vector_t;         

vector_t *VectorCreate(size_t initial_capacity, size_t element_size);

void *VectorGetAccessToElement(vector_t *vector, size_t index);

int VectorAppend(vector_t *vector, const void *value);

void VectorRemoveLast(vector_t *vector);

int VectorIsEmpty(const vector_t *vector);                           

size_t VectorCapacity(const vector_t *vector);

size_t VectorSize(const vector_t *vector);

void VectorDestroy(vector_t *vector);

int VectorReserve(vector_t *vector, size_t size);

int VectorShrink(vector_t *vector);

#endif /* __VECTOR__ */
