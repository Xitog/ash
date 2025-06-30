#include "dict.h"

Dict * dict_init(Type key_type, Type value_type) {
    Dict * d = (Dict *) memory_get(sizeof(Dict));
    d->keys = list_init(key_type);
    d->values = list_init(value_type);
    d->key_type = key_type;
    d->value_type = value_type;
    return d;
}

void dict_free(Dict * dict)
{
    list_free(dict->keys);
    list_free(dict->values);
    memory_free(dict);
}

void dict_set(Dict * dict, Value key, Value value)
{
    if (key.type != dict->key_type)
    {
        general_error("Impossible to set with a key of type %s into a dict with keys of type %s", TYPE_REPR_STRING[key.type], TYPE_REPR_STRING[dict->key_type]);
    }
    Iterator it = iterator_init(dict->keys);
    unsigned int count = 0;
    bool found = false;
    Value current_key = iterator_next(&it);
    while (!is_nil(current_key))
    {
        value_print_message("dict_set :: Test of equality between (current=) %v and (searched=) %v\n", current_key, key);
        if (equality(current_key, key))
        {
            printf("dict_set :: Adding at %d\n", count);
            list_set(dict->values, count, value);
            found = true;
            break;
        }
        count += 1;
        current_key = iterator_next(&it);
    }
    if (!found)
    {
        printf("dict_set :: Adding at the end of dict\n");
        list_append(dict->keys, key);
        list_append(dict->values, value);
    }
}

Value dict_get(Dict * dict, Value key)
{
    if (key.type != dict->key_type)
    {
        general_error("Impossible to search key of type %s into dict with keys of type %s", TYPE_REPR_STRING[key.type], TYPE_REPR_STRING[dict->key_type]);
    }
    Iterator it = iterator_init(dict->keys);
    unsigned int count = 0;
    Value current_key = iterator_next(&it);
    while (!is_nil(current_key))
    {
        if (equality(current_key, key))
        {
            break;
        }
        count += 1;
        current_key = iterator_next(&it);
    }
    return list_get(dict->values, count);
}

size_t dict_size(Dict * dict)
{
    return list_size(dict->keys);
}

bool dict_key_exists(Dict * dict, Value key)
{
    if (key.type != dict->key_type)
    {
        general_error("Impossible to search key of type %s into dict with keys of type %s", TYPE_REPR_STRING[key.type], TYPE_REPR_STRING[dict->key_type]);
    }
    Iterator it = iterator_init(dict->keys);
    bool found = false;
    Value current_key = iterator_next(&it);
    while (!is_nil(current_key))
    {
        if (equality(current_key, key))
        {
            found = true;
            break;
        }
        current_key = iterator_next(&it);
    }
    return found;
}

bool dict_is_empty(Dict * dict)
{
    return list_size(dict->keys) == 0;
}

void dict_print(Dict * dict)
{
    size_t size = list_size(dict->keys);
    printf("Dict <%s => %s> [#%d] :\n", TYPE_REPR_STRING[dict->key_type], TYPE_REPR_STRING[dict->value_type], size);
    for (unsigned int i = 0; i < size; i++)
    {
        Value key = list_get(dict->keys, i);
        Value val = list_get(dict->values, i);
        value_print_message("\t%d. %v => %v\n", i, key, val);
    }
}

//void dict_print_sub(Dict * dict, void (*print)())
//{
    // :TODO:
//}
