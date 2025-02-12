#include "general.h"

unsigned int total_allocated = 0;
unsigned int total_freed = 0;

void *memory_get(unsigned int size)
{
    #if DEBUG
    printf("Asked for %d\n", size);
    #endif
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
    total_freed += _msize(ptr); // special MS
    free(ptr);
}

unsigned int memory_size(void * ptr)
{
    return _msize(ptr);
}

void memory_copy(void * dest, void * source)
{
    memcpy(dest, source, memory_size(source));
}

void memory_summary()
{
    printf("Memory usage: %d allocated %d freed %+d diff\n", total_allocated, total_freed, total_allocated - total_freed);
}

void general_error(char * message)
{
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}
