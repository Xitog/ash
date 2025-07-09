#include "value.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const Value NIL = {.type = VALUE_NIL, .as.any = NULL};

const char *VALUE_TYPE_STRING[] = {
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
    return v.type == VALUE_NIL;
}

bool is_primitive(Value v)
{
    return v.type == VALUE_BOOLEAN || v.type == VALUE_FLOAT || v.type == VALUE_INTEGER || v.type == VALUE_NIL || v.type == VALUE_TYPE;
}

bool type_is_number(ValueType t)
{
    return t == VALUE_INTEGER || t == VALUE_FLOAT;
}

bool strict_equality(Value v1, Value v2)
{
    if (v1.type != v2.type)
    {
        return false;
    }
    if (v1.type == VALUE_INTEGER || v1.type == VALUE_TYPE)
    {
        return v1.as.integer == v2.as.integer;
    }
    else if (v1.type == VALUE_FLOAT)
    {
        return v1.as.real == v2.as.real;
    }
    else if (v1.type == VALUE_BOOLEAN)
    {
        return v1.as.boolean == v2.as.boolean;
    }
    else if (v1.type == VALUE_CSTRING)
    {
        return strcmp(v1.as.cstring, v2.as.cstring) == 0;
    }
    else if (v1.type == VALUE_STRING)
    {
        //printf("strict_equality :: %s vs %s = %d\n", v1.as.string->content, v2.as.string->content, strcmp(v1.as.string->content, v2.as.string->content));
        return strcmp(v1.as.string->content, v2.as.string->content) == 0;
    }
    general_message(FATAL, "Type unknown for strict_equality");
    return false;
}

bool equality(Value v1, Value v2)
{
    if (v1.type == VALUE_INTEGER && v2.type == VALUE_FLOAT)
    {
        return (float)v1.as.integer == v2.as.real;
    }
    else if (v2.type == VALUE_INTEGER && v1.type == VALUE_FLOAT)
    {
        return (float)v2.as.integer == v1.as.real;
    }
    else
    {
        return strict_equality(v1, v2);
    }
}

Value integer_init(long i)
{
    Value v;
    v.type = VALUE_INTEGER;
    v.as.integer = i;
    return v;
}

Value type_init(ValueType t)
{
    Value v;
    v.type = VALUE_TYPE;
    v.as.integer = t;
    return v;
}

Value float_init(float f)
{
    Value v;
    v.type = VALUE_FLOAT;
    v.as.real = f;
    return v;
}

Value boolean_init(bool b)
{
    Value v;
    v.type = VALUE_BOOLEAN;
    v.as.boolean = b;
    return v;
}

Value cstring_init(char *s)
{
    Value v;
    v.type = VALUE_CSTRING;
    v.as.cstring = s;
    return v;
}

void cstring_delete(Value v)
{
    memory_free(v.as.cstring);
}

Value string_init(char *s)
{
    XString * str = memory_get(sizeof(XString));
    str->size = strlen(s);
    str->content = memory_get(sizeof(char) * (str->size + 1));
    str->refcount = 1;
    Value v;
    v.type = VALUE_STRING;
    v.as.string = str;
    for (int i = 0; i < str->size + 1; i++)
    {
        str->content[i] = s[i];
    }
    return v;
}

void string_delete(Value v)
{
    v.as.string->refcount -= 1;
    if (v.as.string->refcount == 0)
    {
        memory_free(v.as.string->content);
        memory_free(v.as.string);
    }
}

void value_print(Value v)
{
    switch (v.type)
    {
    case VALUE_INTEGER:
        long i = v.as.integer;
        printf("%d", i);
        break;
    case VALUE_FLOAT:
        float f = v.as.real;
        printf("%f", f);
        break;
    case VALUE_BOOLEAN:
        bool b = v.as.boolean;
        if (b == true)
        {
            printf("true");
        }
        else
        {
            printf("false");
        }
        break;
    case VALUE_CSTRING:
        printf("%s (#%d)", v.as.cstring, strlen(v.as.cstring));
        break;
    case VALUE_STRING:
        XString *s = v.as.string;
        printf("%s (#%d) (ref=%d)", s->content, s->size, s->refcount);
        break;
    case VALUE_TYPE:
        printf("%s (%d)", VALUE_TYPE_STRING[v.as.integer], v.as.integer);
        break;
    case VALUE_NIL:
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
