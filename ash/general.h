#ifndef __GENERAL__
#define __GENERAL__

#include <stdlib.h>
#include <stdio.h>

extern unsigned int total_allocated;
extern unsigned int total_freed;

void * memory_get(unsigned int size);
void memory_free(void * ptr);
void memory_summary();

#endif
