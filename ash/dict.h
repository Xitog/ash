#ifndef __DICT__
#define __DICT__

//-----------------------------------------------------------------------------
// Imports
//-----------------------------------------------------------------------------

#include "list.h"

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef struct _Dict {
    List * keys;
    List * values;
    Type key_type;
    Type value_type;
} Dict;

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

Dict * dict_init(Type key_type, Type value_type);
void dict_free(Dict * dict);
void dict_set(Dict * dict, Value key, Value value);
Value dict_get(Dict * dict, Value key);
size_t dict_size(Dict * dict);
bool dict_key_exists(Dict * dict, Value key);
bool dict_is_empty(Dict * dict);
void dict_print(Dict * dict);
//void dict_print_sub(Dict * dict, void (*print)());

#endif
