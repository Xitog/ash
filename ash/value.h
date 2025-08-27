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
    VALUE_NIL     = 0,
    VALUE_INTEGER = 1,
    VALUE_FLOAT   = 2,
    VALUE_BOOLEAN = 3,
    VALUE_STRING  = 4,
    VALUE_CSTRING = 5,
    VALUE_ARRAY   = 6,
    VALUE_LIST    = 7,
    VALUE_TABLE   = 8,
    VALUE_RECORD  = 9,
    VALUE_CDATA   = 10,
    VALUE_TYPE    = 11, // autoref
    VALUE_FUNCTION = 12,
    VALUE_ANY     = 13
} ValueType;

typedef struct _String
{
    bool copied;
    long size;
    char * content;
    long refcount;
} XString;

typedef union _Data
{
    int32_t integer;
    float real; // 32
    bool boolean;
    char * cstring;
    XString * string;
    void * any;
} Data;

typedef struct _Value
{
    ValueType type;
    Data as;
} Value;

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

extern const Value THE_NIL;
const extern char *VALUE_TYPE_STRING[];

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

bool is_nil(Value v);
bool is_primitive(Value v);
bool type_is_number(ValueType t);
bool strict_equality(Value v1, Value v2);
bool equality(Value v1, Value v2);
Value integer_init(long i);
Value type_init(ValueType t);
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
