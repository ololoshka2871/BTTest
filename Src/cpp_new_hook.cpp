#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

//
// Define the 'new' operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void *operator new(size_t size){
	void *p=pvPortMalloc(size);
	return p;
}

//
// Define the 'delete' operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void operator delete(void *p){
	vPortFree( p );
}
