#include "value.h"

//-----------------------------------------------------------------------------
// Constantes
//-----------------------------------------------------------------------------

const AshRef NIL = { .type = TYPE_NIL};

//-----------------------------------------------------------------------------
// Fonctions
//-----------------------------------------------------------------------------

AshRef integer_init(long i)
{
    AshRef ref;
    ref.type = TYPE_INTEGER;
    ref.value.i = i;
    return ref;
}

AshRef float_init(float f)
{
    AshRef ref;
    ref.type = TYPE_FLOAT;
    ref.value.f = f;
    return ref;
}

AshRef boolean_init(bool b)
{
    AshRef ref;
    ref.type = TYPE_BOOLEAN;
    ref.value.b = b;
    return ref;
}

AshRef string_init(char *source, long source_size)
{
    AshRef ref;
    ref.type = TYPE_STRING;
    AshString * s = malloc(sizeof(AshString));
    ref.value.p = (void *)s;
    s->size = source_size;
    s->content = malloc(sizeof(char) * source_size);
    for (int i = 0; i < source_size; i++)
    {
        s->content[i] = source[i];
    }
    return ref;
}

void print(AshRef ref)
{
    switch (ref.type)
    {
    case TYPE_INTEGER:
        long i = ref.value.i;
        printf("%d\n", i);
        break;
    case TYPE_FLOAT:
        float f = ref.value.f;
        printf("%f\n", f);
        break;
    case TYPE_BOOLEAN:
        bool b = ref.value.b;
        if (b == true)
        {
            printf("true\n");
        }
        else
        {
            printf("false\n");
        }
        break;
    case TYPE_STRING:
        AshString *s = (AshString *)ref.value.p;
        printf("%s (%d)\n", s->content, s->size);
        break;
    case TYPE_NIL:
        printf("nil\n");
    default:
        printf("ERREUR : TYPE INCONNU\n");
        break;
    }
}
