#ifndef _HSMFUI_H
#define _HSMFUI_H

#include <stdint.h>

/***********************************************************/

#define AR_LEN(ar) sizeof(ar)/sizeof(ar[0])

#define _NODE(name, nchildren, pchildren, orth) \
    Hsmfui name = { \
        #name,                      /* name                 */ \
        NULL,                       /* parent               */ \
        nchildren,                  /* nChildren            */ \
        pchildren,                  /* children             */ \
        NULL, NULL, NULL, NULL,     /* Init, Ent, Act, Exi  */ \
        NULL,                       /* Error                */ \
        orth,                       /* isOrth               */ \
        NULL,                       /* state                */ \
        STATUS_NULL                 /* status               */ \
    };

#define LEAF(name) _NODE(name, 0, NULL, 0)

#define NODE_START(name) \
    Hsmfui * _CHILDREN_##name[] = {

#define ORTH_START(name) NODE_START(name)

#define NODE_STOP(name) \
    }; \
    _NODE(name, AR_LEN(_CHILDREN_##name), _CHILDREN_##name, 0)

#define ORTH_STOP(name) \
    }; \
    _NODE(name, AR_LEN(_CHILDREN_##name), _CHILDREN_##name, 1)

/***********************************************************/

#define HSMFUI_ERRORS \
X( HSMFUI_ERROR_NONE                  ), \
X( HSMFUI_ERROR_DUPLICATE_STATE       ), \
X( HSMFUI_ERROR_DOUBLE_INITIALISATION ), \
X( HSMFUI_ERROR_DOUBLE_ENTRY          ), \
X( HSMFUI_ERROR_DOUBLE_EXIT           ), \
X( HSMFUI_ERROR_INVALID_STATE         ), \
X( HSMFUI_ERROR_SET_ACTIVE_STATE      ), \
X( HSMFUI_ERROR_UNSPECIFIED           )

enum hsmfui_error
{
#define X(error) error
HSMFUI_ERRORS
#undef X
};

enum stateStatus
{
    STATUS_NULL,
    STATUS_INITIALISED,
    STATUS_ENTERED
};
struct Hsmfui;
typedef struct Hsmfui
{
    const char * name;

    Hsmfui * parent;
    
    const int nChildren;
    struct Hsmfui * const (* const children);

    void (*Init)    (void);
    void (*Ent)     (void);
    void (*Act)     (void);
    void (*Exi)     (void);

    void (*Error) (enum hsmfui_error);

    const int isOrth;

    Hsmfui * state;

    enum stateStatus status;
    
} Hsmfui;

extern void hsmfui_Init( Hsmfui * hsm );
extern void hsmfui_Act( Hsmfui * hsm );
extern void hsmfui_Ent( Hsmfui * hsm );
extern void hsmfui_Exi( Hsmfui * hsm );

extern const char * const hsmfui_getErrorString( enum hsmfui_error error);

extern void hsmfui_SetState( Hsmfui * hsm, Hsmfui * state );
extern void hsmfui_Transition( Hsmfui * hsm, Hsmfui * state );


#endif /* _HSMFUI_H */
