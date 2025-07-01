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

unsigned int memory_size(void *ptr)
{
    return _msize(ptr);
}

// On initialise un objet mémoire à partir d'une char * (probablement non supervisée par notre système)
void *memory_get_from_str(char * source)
{
    size_t s = strlen(source) + 1; // for \0 char
    void * p = memory_get(s);
    memcpy(p, (void *) source, s);
    return p;
}

void memory_copy(void *dest, void *source)
{
    memcpy(dest, source, memory_size(source));
}

void memory_summary()
{
    printf("Memory usage: %d allocated %d freed %+d diff\n", total_allocated, total_freed, total_allocated - total_freed);
}

void general_message(ErrorLevel lvl, char *message, ...)
{
    va_list args;
    va_start(args, message); // enable the variable arguments after message parameter
    uint32_t c = 0;
    size_t length = strlen(message);
    printf("ERROR: ");
    while (c < length)
    {
        if (message[c] == '%' && c + 1 < length)
        {
            if (message[c + 1] == 'd')
            {
                int i = va_arg(args, int);
                printf("%d", i);
                c++;
            }
            else if (message[c + 1] == 'f')
            {
                double d = va_arg(args, double);
                printf("%f", d);
                c++;
            }
            else if (message[c + 1] == 's')
            {
                char * s = va_arg(args, char *);
                printf("%s", s);
                c++;
            }
            else if (message[c + 1] == 't')
            {
                Token t = va_arg(args, Token);
                token_print(t);
                c++;

            }
            else
            {
                printf("%c", message[c]);
            }
        }
        else
        {
            printf("%c", message[c]);
        }
        c++;
    }
    va_end(args);
    printf("\n");
    if (lvl == FATAL)
    {
        exit(EXIT_FAILURE);
    }
}

// unsused
char *string_copy(const char *source)
{
    size_t size = _msize((void *)source);
    char *dest = memory_get(size * sizeof(char));
    strncpy_s(dest, size, source, size);
    return dest;
}
