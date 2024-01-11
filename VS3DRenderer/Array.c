#include <stdio.h>
#include <stdlib.h>
#include "Array.h"

#define ARRAY_RAW_DATA(Array) ((int*)(Array) - 2)
#define ARRAY_CAPACITY(Array) (ARRAY_RAW_DATA(Array)[0])
#define ARRAY_OCCUPIED(Array) (ARRAY_RAW_DATA(Array)[1])

void * Array_Hold(void * Array, int Count, int ItemSize)
{
	if (Array == NULL)
	{
		int raw_size = (sizeof(int) * 2) + (ItemSize * Count);
		int* base = (int*)malloc(raw_size);
		base[0] = Count;  // capacity
		base[1] = Count;  // occupied
		return base + 2;
	}
	else if (ARRAY_OCCUPIED(Array) + Count <= ARRAY_CAPACITY(Array)) 
	{
		ARRAY_OCCUPIED(Array) += Count;
		return Array;
	}
	else 
	{
		int needed_size = ARRAY_OCCUPIED(Array) + Count;
		int float_curr = ARRAY_CAPACITY(Array) * 2;
		int capacity = needed_size > float_curr ? needed_size : float_curr;
		int occupied = needed_size;
		int raw_size = sizeof(int) * 2 + ItemSize * capacity;
		int* base = (int*)realloc(ARRAY_RAW_DATA(Array), raw_size);
		base[0] = capacity;
		base[1] = occupied;
		return base + 2;
	}
}

int Array_Length(void * Array)
{
	return (Array != NULL) ? ARRAY_OCCUPIED(Array) : 0;
}

void Array_Free(void * Array)
{
	if (Array != NULL) 
	{
		free(ARRAY_RAW_DATA(Array));
	}
}
