#pragma once
#ifndef ARRAY_H
#define ARRAY_H

// Push new values into the array
#define Array_Push(Array, Value)                                              \
    do																		  \
	{                                                                         \
        (Array) = Array_Hold( (Array), 1, sizeof(*(Array)) );                   \
        (Array)[Array_Length(Array) - 1] = (Value);                           \
    } while (0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hold the elements inside the array
void* Array_Hold(void* Array, int Count, int ItemSize);

// Return how many elements has the array
int Array_Length(void* Array);

// Free the allocated memory for the array
void Array_Free(void* Array);



#endif // !ARRAY_H

