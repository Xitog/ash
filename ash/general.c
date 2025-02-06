#include "general.h"

unsigned int total_allocated = 0;
unsigned int total_freed = 0;

void *memory_get(unsigned int size)
{
    void *ptr = malloc(size);
    if (ptr != NULL)
    {
        total_allocated += size;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

void memory_free(void *ptr)
{
    free(ptr);
    total_freed += _msize(ptr); // special MS
}

void memory_summary()
{
    printf("Memory usage: %d allocated %d freed %d diff", total_allocated, total_freed, total_allocated - total_freed);
}
