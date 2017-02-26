#include "Hsmfui.h"
#include <stdio.h>

static void doForActive( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) );
static void doForAll   ( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) );

static void setChildrensParent( Hsmfui * hsm );
static void activateFirstChild( Hsmfui * hsm );

static void doInit( Hsmfui * hsm );
static void doEnt( Hsmfui * hsm );
static void doAct( Hsmfui * hsm );
static void doExi( Hsmfui * hsm );

static int countSubstate( Hsmfui * hsm, Hsmfui * state );
static int hasDuplicate(  Hsmfui * top, Hsmfui * subSm );

static void raiseError( Hsmfui * hsm, enum hsmfui_error error );

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
    if( hasDuplicate( hsm, hsm ) )
    {
        /* do not use raise error as parents are not defined yet */
        hsm->Error( HSMFUI_ERROR_DUPLICATE_STATE );
    }
    else
    {
        doForAll( hsm, setChildrensParent );
        doForAll( hsm, activateFirstChild );
        doForAll( hsm, doInit );
    }
}

void hsmfui_Ent( Hsmfui * hsm )
{
    doForActive( hsm, doEnt );
}

void hsmfui_Act( Hsmfui * hsm )
{
    doForActive( hsm, doAct );
}

void hsmfui_Exi( Hsmfui * hsm )
{
    doForActive( hsm, doExi );
}

const char const * hsmfui_getErrorString( enum hsmfui_error error)
{
    return errorStrings[error];
}

void hsmfui_SetState( Hsmfui * hsm, Hsmfui * state )
{
}

void hsmfui_Transition( Hsmfui * hsm, Hsmfui * state )
{
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

static void doInit( Hsmfui * hsm )
{
    void (*handler)(void);

    if( hsm->status == STATUS_NULL )
    {
        handler = hsm->Init;
        if( handler != NULL) handler();
        hsm->status = STATUS_INITIALISED;
    }
    else
    {
        raiseError( hsm, HSMFUI_ERROR_DOUBLE_INITIALISATION );
    }
}

static void doEnt( Hsmfui * hsm )
{
    void (*handler)(void);

    handler = hsm->Ent;
    if( handler != NULL) handler();
}

static void doAct( Hsmfui * hsm )
{
    void (*handler)(void);

    handler = hsm->Act;
    if( handler != NULL) handler();
}

static void doExi( Hsmfui * hsm )
{
    void (*handler)(void);

    handler = hsm->Exi;
    if( handler != NULL) handler();
}

static int countSubstate( Hsmfui * hsm, Hsmfui * state)
{
    int i;
    Hsmfui * child;
    int sum = 0;

    if ( hsm == state ) sum++;

    for(i = 0; i < hsm->nChildren; i++)
    {
        child = hsm->children[i];
        sum += countSubstate( child, state );
        if( sum > 1 ) break;    /* prevent infinite loop */
    }
    return sum;
}

static int hasDuplicate( Hsmfui * top, Hsmfui * subSm )
{
    int i;
    Hsmfui * child;
    int result = 0;

    if ( countSubstate( top, subSm ) > 1 )
    {
        result = 1;
    }
    else
    {
        for(i = 0; i < subSm->nChildren; i++)
        {
            child = subSm->children[i];
            if( hasDuplicate( top, child ) )
            {
                result = 1;
                break;    /* prevent infinite loop */
            }
        }
    }

    #ifdef DEBUG
    /* printf("%s(%s,%s) = %d\n", __FUNCTION__, top->name, subSm->name, result); */
    #endif
    
    return result;
}

static void raiseError( Hsmfui * hsm, enum hsmfui_error error )
{
    if( hsm->Error != NULL )
    {
        hsm->Error(error);
    }
    else
    {
        /* segfault when no parent as there is no handler for errors anyway in that case*/
        raiseError( hsm->parent, error );
    }
}
