#ifndef _HSMFUI_H
#define _HSMFUI_H

#include <stdint.h>

/***********************************************************/

#define AR_LEN(ar) sizeof(ar)/sizeof(ar[0])

#define _NODE(name, nchildren, pchildren, orth) \
    Hsmfui name = { \
        #name,                      /* Name                 */ \
        NULL,                       /* Parent               */ \
        nchildren,                  /* NChildren            */ \
        pchildren,                  /* Children             */ \
        NULL, NULL, NULL, NULL,     /* Init, Ent, Act, Exi  */ \
        NULL,                       /* Error                */ \
        orth,                       /* IsOrth               */ \
        NULL,                       /* State                */ \
        STATUS_NULL                 /* Status               */ \
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
X( HSMFUI_ERROR_NONE                         ), \
X( HSMFUI_ERROR_DUPLICATE_STATE              ), \
X( HSMFUI_ERROR_DOUBLE_INITIALISATION        ), \
X( HSMFUI_ERROR_DOUBLE_ENTRY                 ), \
X( HSMFUI_ERROR_DOUBLE_EXIT                  ), \
X( HSMFUI_ERROR_INVALID_STATE                ), \
X( HSMFUI_ERROR_SET_ON_WRONG_STATUS          ), \
X( HSMFUI_ERROR_TRANSITION_ON_WRONG_STATUS   ), \
X( HSMFUI_ERROR_ACT_WITHOUT_ENT              ), \
X( HSMFUI_ERROR_UNSPECIFIED                  )

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
typedef struct Hsmfui Hsmfui;
struct Hsmfui
{
    const char *    Name;

    Hsmfui *        Parent;

    const int       NChildren;
    Hsmfui * const (* const Children);

    void (*Init) (void);
    void (*Ent)  (void);
    void (*Act)  (void);
    void (*Exi)  (void);

    void (*Error) (enum hsmfui_error);

    const int        IsOrth;

    Hsmfui *         State;

    enum stateStatus Status;

};

extern void hsmfui_Init( Hsmfui * hsm );
extern void hsmfui_Act( Hsmfui * hsm );
extern void hsmfui_Ent( Hsmfui * hsm );
extern void hsmfui_Exi( Hsmfui * hsm );

extern const char * const hsmfui_getErrorString( enum hsmfui_error error);

extern void hsmfui_SetState( Hsmfui * hsm, Hsmfui * state );
extern void hsmfui_Transition( Hsmfui * hsm, Hsmfui * state );


#endif /* _HSMFUI_H */
