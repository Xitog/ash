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
    TYPE_CSTRING = 5,
    TYPE_ARRAY   = 6,
    TYPE_LIST    = 7,
    TYPE_TABLE   = 8,
    TYPE_RECORD  = 9,
    TYPE_CDATA   = 10,
    TYPE_TYPE    = 11, // autoref
    TYPE_ANY     = 12
} Type;

typedef struct _String
{
    bool copied;
    long size;
    char * content;
    long refcount;
} XString;

typedef union _Data
{
    int32_t as_int;
    float as_float; // 32
    bool as_bool;
    char * as_cstring;
    XString * as_string;
    void * as_any;
} Data;

typedef struct _Value
{
    Type type;
    Data value;
} Value;

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
Value type_init(Type t);
Value float_init(float f);
Value boolean_init(bool b);
Value cstring_init(char *s);
void cstring_delete(Value v);
Value string_init(char * source);
Value string_init_copy(char *s);
void string_delete(Value v);
void value_print(Value val);
void value_print_message(char *message, ...);

#endif
