#ifndef __GENERAL__
#define __GENERAL__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "token.h"

typedef enum _ErrorLevel
{
    EL_DEBUG = 0,
    LOG = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4
} ErrorLevel;

extern ErrorLevel display_error_level;

typedef struct _DynArray
{
    void *data;
    uint32_t count;
    uint32_t capacity;
    size_t element_size;
} DynArray;

extern unsigned int total_allocated;
extern unsigned int total_freed;

void set_display_error(ErrorLevel lvl);
void *memory_get(unsigned int size);
void memory_free(void *ptr);
unsigned int memory_size(void *ptr);
void *memory_get_from_str(char *source);
void memory_copy(void *dest, void *source);
void memory_summary();
void general_message(ErrorLevel lvl, char *message, ...);
char *string_copy(const char *source);

// Dynamic array

#define CAPACITY_BASE 8
#define CAPACITY_MULTIPLIER 2

DynArray dyn_array_init_preset(size_t element_size, uint32_t length);
DynArray dyn_array_init(size_t element_size);
void dyn_array_free(DynArray *da);
void dyn_array_append(DynArray *da, void *d);
void dyn_array_append_sorted(DynArray *da, void *data, bool (*is_sup)(void *e1, void *e2));
void dyn_array_insert(DynArray *da, void *data, uint32_t index);
void *dyn_array_get(DynArray da, int32_t index);
void dyn_array_delete(DynArray *da, int32_t index);
void dyn_array_info(DynArray da, void (*display)(void *element));
uint32_t dyn_array_size(DynArray da);
void dyn_array_clear(DynArray *da);

#endif
