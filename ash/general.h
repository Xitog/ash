#ifndef __GENERAL__
#define __GENERAL__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "token.h"

extern unsigned int total_allocated;
extern unsigned int total_freed;

void * memory_get(unsigned int size);
void memory_free(void * ptr);
unsigned int memory_size(void * ptr);
void memory_copy(void * dest, void * source);
void memory_summary();
void general_error(char * message, ...);
char * string_copy(const char *source);

#endif
