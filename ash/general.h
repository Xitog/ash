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
    LOG   = 1,
    WARN  = 2,
    ERROR = 3,
    FATAL = 4
} ErrorLevel;

extern ErrorLevel display_error_level;
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

#endif
