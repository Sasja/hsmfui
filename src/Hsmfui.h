#ifndef _HSMFUI_H
#define _HSMFUI_H

#include <stdint.h>

#define AR_LEN(ar) sizeof(ar)/sizeof(ar[0])

#define _NODE(name, nchildren, pchildren, orth) \
    Hsmfui name = { \
        #name, \
        NULL, \
        nchildren, \
        pchildren, \
        NULL, NULL, NULL, NULL, NULL, NULL, \
        orth, \
        NULL \
    };

#define LEAF(name) _NODE(name, 0, NULL, 0)

#define NODE_START(name) \
    Hsmfui * _CHILDREN_##name[] = {

#define ORTH_START(name) NODE_START(name)

#define CHILD(child) &child ,

#define NODE_STOP(name) \
    }; \
    _NODE(name, AR_LEN(_CHILDREN_##name), _CHILDREN_##name, 0)

#define ORTH_STOP(name) \
    }; \
    _NODE(name, AR_LEN(_CHILDREN_##name), _CHILDREN_##name, 1)

typedef struct Hsmfui Hsmfui;
typedef struct Hsmfui
{
    const char * name;

    Hsmfui * parent;
    
    const int nChildren;
    Hsmfui * const (* const children);

    void (*Init)    (void);
    void (*Ent)     (void);
    void (*Act)     (void);
    void (*Exi)     (void);
    void (*Guard)   (void);
    void (*Proceed) (void);

    const int isOrth;

    Hsmfui * state;
    
} Hsmfui;

extern void hsmfui_Init( Hsmfui * hsm );
extern void hsmfui_Act( Hsmfui * hsm );
extern void hsmfui_Ent( Hsmfui * hsm );
extern void hsmfui_Exi( Hsmfui * hsm );

int8_t AverageThreeBytes(int8_t a, int8_t b, int8_t c);

#endif /* _HSMFUI_H */
