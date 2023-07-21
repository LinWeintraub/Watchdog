/* ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
 ~ Author        Lin Weintraub                                   ~
 * Version       0.0.1                                           *
 ~ Date          3/10/22                                         ~
 * Description   Dynamic Vector                                  *
 * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ */
 
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy */
#include <assert.h> /* assert */

#include "vector.h"

#define GROWTH_FACTOR 2

struct vector
{
    void *vector_array;
    size_t capacity;
    size_t element_size;
    size_t element_total;
};

vector_t *VectorCreate(size_t initial_capacity, size_t element_size)
{

	vector_t *vector = malloc(sizeof(vector_t));
	if (NULL == vector)
	{
		return NULL;
	}
	
	vector->vector_array = malloc(element_size * initial_capacity);
	if (NULL == vector->vector_array)
	{
		free(vector);
		return NULL;
	}
	
	vector->element_total = 0;
	vector->capacity = initial_capacity;
	vector->element_size = element_size;
    
    return vector;
}

void *VectorGetAccessToElement(vector_t *vector, size_t index)
{
	char *next_element = (char *)(vector->vector_array) + (index * (vector->element_size));
	
	assert(vector);
	
	if (index > vector->element_total)
	{
		return NULL;
	}
	
    return next_element;
}

int VectorAppend(vector_t *vector, const void *value)
{
	int status = 0;
	char *next_element = (char *)(vector->vector_array) + ((vector->element_total) * (vector->element_size));
	
	assert(vector);
	
	if (vector->element_total == vector->capacity)
	{
		status = VectorReserve(vector, vector->element_total);
		
		if (0 != status)
    	{
    		return 1;
    	}
	}
	
	memcpy(next_element, value, vector->element_size);
	
	++(vector->element_total);
	
	return status;
}

void VectorRemoveLast(vector_t *vector)
{
	assert(vector);
	assert(0 != vector->element_total);
	
	--(vector->element_total);
}

int VectorIsEmpty(const vector_t *vector)
{
	assert(vector);
	
	return (0 == vector->element_total);
}                      

size_t VectorCapacity(const vector_t *vector)
{
	assert(vector);
	
	return (vector->capacity);
}

size_t VectorSize(const vector_t *vector)
{
	assert(vector);
	
	return (vector->element_total);
}

void VectorDestroy(vector_t *vector)
{
	assert(vector);
	
	free(vector->vector_array); vector->vector_array = NULL;
	free(vector); vector = NULL;
}

int VectorReserve(vector_t *vector, size_t size)
{
	void *vector_array_buff = realloc(vector->vector_array, ((vector->element_size) * (vector->capacity + size)));
    if (NULL == vector_array_buff) 
    {
		return 1;
    }
    
	assert(vector);
	
    vector->vector_array = vector_array_buff;       
    vector->capacity += size;
    
    return 0;
}

int VectorShrink(vector_t *vector)
{
	int status = 0;
	size_t temp_capacity = 0;
	
	assert(vector);
	
	if ((vector->element_total) < ((vector->capacity) / GROWTH_FACTOR))
	{
    	temp_capacity = (vector->element_total) * GROWTH_FACTOR;
    	status = VectorReserve(vector, vector->capacity);
    	
    	if (0 != status)
    	{
    		return 1;
    	}
    	
		vector->capacity = temp_capacity;
	}
	
	return 0;
}
