#ifndef __VALUE__
#define __VALUE__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "general.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef enum
{
    TYPE_INTEGER = 1,
    TYPE_FLOAT   = 2,
    TYPE_BOOLEAN = 3,
    TYPE_STRING  = 4,
    TYPE_ARRAY   = 5,
    TYPE_LIST    = 6,
    TYPE_TABLE   = 7,
    TYPE_RECORD  = 8,
    TYPE_NIL     = 9,
    TYPE_CDATA   = 10
} AshType;

typedef union
{
    long   i;   // 32
    float  f;   // 32
    bool   b;
    void * p;
} AshValue;

typedef struct
{
    AshType type;
    AshValue value;
} AshRef;

typedef struct
{
    long size;
    char * content;
} AshString;

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

extern const AshRef NIL;

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

AshRef integer_init(long i);
AshRef float_init(float f);
AshRef boolean_init(bool b);
AshRef string_init(char * source, long source_size);
void print(AshRef ref);

#endif
