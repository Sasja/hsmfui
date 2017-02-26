#include "Hsmfui.h"
#include <stdio.h>

static void doForActive( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) );
static void doForAll   ( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) );

static void setChildrensParent( Hsmfui * hsm );
static void activateFirstChild( Hsmfui * hsm );

static void callInit( Hsmfui * hsm );
static void callEnt( Hsmfui * hsm );
static void callAct( Hsmfui * hsm );
static void callExi( Hsmfui * hsm );

/***********************************************************/

const char const * errorStrings[] =
{
#define X(error) #error,
HSMFUI_ERRORS
#undef X
};

/***********************************************************/

void hsmfui_Init( Hsmfui * hsm )
{
    doForAll( hsm, setChildrensParent );
    doForAll( hsm, activateFirstChild );
    doForAll( hsm, callInit );
}

void hsmfui_Ent( Hsmfui * hsm )
{
    doForActive( hsm, callEnt );
}

void hsmfui_Act( Hsmfui * hsm )
{
    doForActive( hsm, callAct );
}

void hsmfui_Exi( Hsmfui * hsm )
{
    doForActive( hsm, callExi );
}

const char const * hsmfui_getErrorString( enum hsmfui_error error)
{
    return errorStrings[error];
}

/***********************************************************/

static void doForActive( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) )
{
    int i;
    Hsmfui * child;

    f( hsm );

    if( hsm->isOrth )
    {
        for(i = 0; i < hsm->nChildren; i++)
        {
            child = hsm->children[i];
            doForActive( child, f );
        }
    }
    else 
    {
        child = hsm->state;
        if( child != NULL ) doForActive( child, f );
    }
}

static void doForAll( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) )
{
    int i;
    Hsmfui * child;

    f( hsm );

    for(i = 0; i < hsm->nChildren; i++)
    {
        child = hsm->children[i];
        doForAll( child, f );
    }
}

/***********************************************************/

static void setChildrensParent( Hsmfui * hsm )
{
    int i;
    Hsmfui * child;
    for( i = 0; i < hsm->nChildren; i++ )
    {
        child = hsm->children[i];
        child->parent = hsm;
    }
}

static void activateFirstChild( Hsmfui * hsm )
{
    if( !hsm->isOrth && hsm->nChildren > 0 ) hsm->state = hsm->children[0];
}

static void callInit( Hsmfui * hsm )
{
    void (*handler)(void);

    handler = hsm->Init;
    if( handler != NULL) handler();
}

static void callEnt( Hsmfui * hsm )
{
    void (*handler)(void);

    handler = hsm->Ent;
    if( handler != NULL) handler();
}

static void callAct( Hsmfui * hsm )
{
    void (*handler)(void);

    handler = hsm->Act;
    if( handler != NULL) handler();
}

static void callExi( Hsmfui * hsm )
{
    void (*handler)(void);

    handler = hsm->Exi;
    if( handler != NULL) handler();
}
