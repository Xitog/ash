#include "value.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const Value NIL = {.type = TYPE_NIL, .value.as_any = NULL};

const char *TYPE_REPR_STRING[] = {
    "NIL",
    "INTEGER",
    "FLOAT",
    "BOOLEAN",
    "STRING",
    "CSTRING",
    "ARRAY",
    "LIST",
    "TABLE",
    "RECORD",
    "CDATA",
    "TYPE",
    "ANY"};

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

bool is_nil(Value v)
{
    return v.type == TYPE_NIL;
}

bool is_primitive(Value v)
{
    return v.type == TYPE_BOOLEAN || v.type == TYPE_FLOAT || v.type == TYPE_INTEGER || v.type == TYPE_NIL || v.type == TYPE_TYPE;
}

bool strict_equality(Value v1, Value v2)
{
    if (v1.type != v2.type)
    {
        return false;
    }
    if (v1.type == TYPE_INTEGER)
    {
        return v1.value.as_int == v2.value.as_int;
    }
    else if (v1.type == TYPE_FLOAT)
    {
        return v1.value.as_float == v2.value.as_float;
    }
    else if (v1.type == TYPE_BOOLEAN)
    {
        return v1.value.as_bool == v2.value.as_bool;
    }
    else if (v1.type == TYPE_CSTRING)
    {
        return strcmp(v1.value.as_cstring, v2.value.as_cstring) == 0;
    }
    else if (v1.type == TYPE_STRING)
    {
        //printf("strict_equality :: %s vs %s = %d\n", v1.value.as_string->content, v2.value.as_string->content, strcmp(v1.value.as_string->content, v2.value.as_string->content));
        return strcmp(v1.value.as_string->content, v2.value.as_string->content) == 0;
    }
    general_error("Type unknown for strict_equality");
    return false;
}

bool equality(Value v1, Value v2)
{
    if (v1.type == TYPE_INTEGER && v2.type == TYPE_FLOAT)
    {
        return (float)v1.value.as_int == v2.value.as_float;
    }
    else if (v2.type == TYPE_INTEGER && v1.type == TYPE_FLOAT)
    {
        return (float)v2.value.as_int == v1.value.as_float;
    }
    else
    {
        return strict_equality(v1, v2);
    }
}

Value integer_init(long i)
{
    Value v;
    v.type = TYPE_INTEGER;
    v.value.as_int = i;
    return v;
}

Value type_init(Type t)
{
    Value v;
    v.type = TYPE_TYPE;
    v.value.as_int = t;
    return v;
}

Value float_init(float f)
{
    Value v;
    v.type = TYPE_FLOAT;
    v.value.as_float = f;
    return v;
}

Value boolean_init(bool b)
{
    Value v;
    v.type = TYPE_BOOLEAN;
    v.value.as_bool = b;
    return v;
}

Value cstring_init(char *s)
{
    Value v;
    v.type = TYPE_CSTRING;
    v.value.as_cstring = s;
    return v;
}

void cstring_delete(Value v)
{
    memory_free(v.value.as_cstring);
}

Value string_init(char *s)
{
    XString * str = memory_get(sizeof(XString));
    str->size = strlen(s);
    str->content = memory_get(sizeof(char) * (str->size + 1));
    str->refcount = 1;
    Value v;
    v.type = TYPE_STRING;
    v.value.as_string = str;
    for (int i = 0; i < str->size + 1; i++)
    {
        str->content[i] = s[i];
    }
    return v;
}

void string_delete(Value v)
{
    v.value.as_string->refcount -= 1;
    if (v.value.as_string->refcount == 0)
    {
        memory_free(v.value.as_string->content);
        memory_free(v.value.as_string);
    }
}

void value_print(Value v)
{
    switch (v.type)
    {
    case TYPE_INTEGER:
        long i = v.value.as_int;
        printf("%d", i);
        break;
    case TYPE_FLOAT:
        float f = v.value.as_float;
        printf("%f", f);
        break;
    case TYPE_BOOLEAN:
        bool b = v.value.as_bool;
        if (b == true)
        {
            printf("true");
        }
        else
        {
            printf("false");
        }
        break;
    case TYPE_CSTRING:
        printf("%s (#%d)", v.value.as_cstring, strlen(v.value.as_cstring));
        break;
    case TYPE_STRING:
        XString *s = v.value.as_string;
        printf("%s (#%d) (ref=%d)", s->content, s->size, s->refcount);
        break;
    case TYPE_NIL:
        printf("nil");
        break;
    default:
        printf("ERREUR : TYPE INCONNU %d\n", v.type);
        break;
    }
}

void value_print_message(char *message, ...)
{
    va_list args;
    va_start(args, message); // enable the variable arguments after message parameter
    uint32_t c = 0;
    size_t length = strlen(message);
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
            else if (message[c + 1] == 'v')
            {
                Value val = va_arg(args, Value);
                value_print(val);
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
}
