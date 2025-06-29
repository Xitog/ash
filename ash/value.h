#ifndef __VALUE__
#define __VALUE__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include "general.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef enum
{
    TYPE_NIL     = 0,
    TYPE_INTEGER = 1,
    TYPE_FLOAT   = 2,
    TYPE_BOOLEAN = 3,
    TYPE_STRING  = 4,
    TYPE_ARRAY   = 5,
    TYPE_LIST    = 6,
    TYPE_TABLE   = 7,
    TYPE_RECORD  = 8,
    TYPE_CDATA   = 9,
    TYPE_TYPE    = 10, // autoref
    TYPE_ANY     = 11
} Type;

typedef union
{
    int32_t as_int;
    float as_float; // 32
    bool as_bool;
    char * as_cstring;
    void * as_any;
} Data;

typedef struct
{
    Type type;
    Data value;
} Value;

typedef struct
{
    long size;
    char * content;
} AshString;

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

extern const Value NIL;
const extern char *TYPE_REPR_STRING[];

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

bool is_nil(Value v);
bool is_primitive(Value v);
bool strict_equality(Value v1, Value v2);
bool equality(Value v1, Value v2);
Value integer_init(long i);
Value float_init(float f);
Value boolean_init(bool b);
Value string_wrapper(char * source);
void string_wrapper_delete(Value v);
Value string_init(char * source, long source_size);
void value_print(Value val);
void value_print_message(char *message, ...);

#endif
