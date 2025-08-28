#define DEBUG

#include "general.h"

ErrorLevel display_error_level = LOG;
unsigned int total_allocated = 0;
unsigned int total_freed = 0;

void set_display_error(ErrorLevel lvl)
{
    display_error_level = lvl;
}

void *memory_get(unsigned int size)
{
    // #if DEBUG
    //  printf("Asked for %d\n", size);
    // #endif
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
void *memory_get_from_str(char *source)
{
    size_t s = strlen(source) + 1; // for \0 char
    void *p = memory_get(s);
    memcpy(p, (void *)source, s);
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
    if (lvl < display_error_level)
    {
        return; // do not display if it is filtered by set_display_error
    }
    va_list args;
    va_start(args, message); // enable the variable arguments after message parameter
    uint32_t c = 0;
    size_t length = strlen(message);
    if (lvl >= 3)
    {
        printf("ERROR: ");
    }
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
                char *s = va_arg(args, char *);
                printf("%s", s);
                c++;
            }
            else if (message[c + 1] == 't')
            {
                Token t = va_arg(args, Token);
                token_print(t);
                c++;
            }
            else if (message[c + 1] == '$')
            {
                TextPart tp = va_arg(args, TextPart);
                text_part_print(tp);
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

DynArray dyn_array_init(size_t element_size)
{
#ifdef DEBUG
    printf("[dyn_array_init] Creating a new dynamic array with %d element size.\n", element_size);
#endif
    DynArray da;
    da.capacity = 8;
    da.count = 0;
    da.data = malloc(element_size * da.capacity);
    da.element_size = element_size;
    return da;
}

void dyn_array_free(DynArray *da)
{
    da->count = 0;
    da->capacity = 0;
    free(da->data);
}

void dyn_array_append(DynArray *da, void *data)
{
    if (da->count + 1 == da->capacity)
    {
        da->capacity = da->capacity * 2;
        da->data = realloc(da->data, da->element_size * da->capacity);
        if (da->data == NULL)
        {
            general_message(FATAL, "Out of memory when reallocating TokenDynArray.");
        }
    }
    memcpy((char *)da->data + da->count * da->element_size, data, da->element_size);
    da->count += 1;
}

void dyn_array_append_sorted(DynArray *da, void *data, bool (*is_sup)(void *e1, void *e2))
{
    if (da->count == 0)
    {
#ifdef DEBUG
        printf("[dyn_array_append_sorted] The dynamic array is empty -> append called\n");
#endif
        dyn_array_append(da, data);
    }
    else
    {
#ifdef DEBUG
        printf("[dyn_array_append_sorted] The dynamic array is not empty: %d/%d\n", da->count, da->capacity);
#endif
        // Passing all elements until the element is not superior to the current
        bool inserted = false;
        uint32_t size = da->count;
        for (uint32_t i = 0; i < size; i++)
        {
#ifdef DEBUG
            printf("[dyn_array_append_sorted] Getting element %d\n", i);
#endif
            void *elt_test = dyn_array_get(*da, i);
#ifdef DEBUG
            printf("[dyn_array_append_sorted] Element %d fetched at %p\n", i, elt_test);
#endif
            if (is_sup(elt_test, data))
            {
#ifdef DEBUG
                printf("[dyn_array_append_sorted] Element fetched is superior to inserted element, insert called at %d\n", i);
#endif
                dyn_array_insert(da, data, i);
                inserted = true;
                break;
            }
        }
        if (!inserted)
        {
#ifdef DEBUG
            printf("[dyn_array_append_sorted] Not superior element found in the dynamic array -> append called\n");
#endif
            dyn_array_append(da, data);
        }
    }
}

void dyn_array_insert(DynArray *da, void *data, uint32_t index)
{
    if (index > da->count + 1)
    {
        general_message(FATAL, "Index %d out of bound (<=%d)", index, da->count);
    }
    // Enlarging dynamic array if necessary
    if (da->count + 1 == da->capacity)
    {
        da->capacity = da->capacity * 2;
        da->data = realloc(da->data, da->element_size * da->capacity);
        if (da->data == NULL)
        {
            general_message(FATAL, "Out of memory when reallocating TokenDynArray.");
        }
    }
#ifdef DEBUG
    for (uint32_t i = 0; i < da->count; i++)
    {
        printf("%d. %p\n", i, (char *)da->data + i);
    }
#endif
    // On pousse
    char *dest = (char *)da->data + (index + 1) * da->element_size;
    char *src = (char *)da->data + index * da->element_size;
#ifdef DEBUG
    printf("[dyn_array_insert] pushing from source : %p to destination : %p\n", src, dest);
#endif
    memmove(dest, src, da->element_size * (da->count - index + 1));
    // On copie
#ifdef DEBUG
    printf("[dyn_array_insert] copying inserted element at %p to destination : %p\n", data, src);
#endif
    memmove(src, data, da->element_size);
    da->count += 1;
}

void *dyn_array_get(DynArray da, int32_t index)
{
    if (index >= 0)
    {
        if (index >= (int64_t)da.count)
        {
            general_message(FATAL, "get : Element at index %d is not defined (count = %d)", index, da.count);
        }
        return (char *)da.data + index * da.element_size;
    }
    else
    {
        if (index < -(int64_t)da.count)
        {
            general_message(FATAL, "get : Element at index %d is not defined (count = %d)", index, da.count);
        }
        return (char *)da.data + (da.count + index) * da.element_size;
    }
}

void dyn_array_delete(DynArray *da, int32_t index)
{
    if (index >= 0)
    {
        if (index >= (int64_t)da->count)
        {
            general_message(FATAL, "delete : Element at index %d is not defined (count = %d)", index, da->count);
        }
        if (index != ((int64_t)da->count) - 1)
        {
            char *dest = (char *)da->data + (index * da->element_size);
            char *src = (char *)da->data + (index + 1) * da->element_size;
            // printf("Moving from %p to %p\n", src, dest);
            memmove(dest, src, da->element_size * (da->count - index + 1));
        }
        // Si on veut supprimer le dernier élément, il suffit d'enlever 1 au compteur
        da->count -= 1;
    }
}

void dyn_array_info(DynArray da, void (*display)(void *element))
{
    printf("DynArray @%p data@%p data#%u %u/%u (element size = %u)\n", &da, da.data, _msize(da.data), da.count, da.capacity, da.element_size);
    uint32_t size = dyn_array_size(da);
    for (uint32_t i = 0; i < size; i++)
    {
        printf("    %03d.", i);
        display(dyn_array_get(da, i));
        printf("\n");
    }
}

uint32_t dyn_array_size(DynArray da)
{
    return da.count;
}
