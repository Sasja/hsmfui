#include "Hsmfui.h"
#include <stdio.h>

static void doForActive( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) );
static void doForAll   ( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) );

static void setChildrensParent( Hsmfui * hsm );
static void activateFirstChild( Hsmfui * hsm );
static void setErrorHandler( Hsmfui * hsm );

static void doInit( Hsmfui * hsm );
static void doEnt( Hsmfui * hsm );
static void doAct( Hsmfui * hsm );
static void doExi( Hsmfui * hsm );

static void defaultErrorHandler( enum hsmfui_error );

static int isChild( Hsmfui * father, Hsmfui * child);
static int countSubstate( Hsmfui * hsm, Hsmfui * state );
static int hasDuplicate(  Hsmfui * top, Hsmfui * subSm );

/***********************************************************/

const char * const errorStrings[] =
{
#define X(error) #error
HSMFUI_ERRORS
#undef X
};

/***********************************************************/

void hsmfui_Init( Hsmfui * hsm )
{
    if( hasDuplicate( hsm, hsm ) )
    {
        hsm->Error( HSMFUI_ERROR_DUPLICATE_STATE );
    }
    else
    {
        doForAll( hsm, setChildrensParent );
        doForAll( hsm, setErrorHandler );
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

static void defaultErrorHandler( enum hsmfui_error error )
{
    /* scratch your nose */
}

const char * const hsmfui_getErrorString( enum hsmfui_error error)
{
    return errorStrings[error];
}

void hsmfui_SetState( Hsmfui * hsm, Hsmfui * state )
{
    if( hsm->Status == STATUS_INITIALISED )
    {
        if( !hsm->IsOrth && isChild(hsm, state) )
        {
            hsm->State = state;
        }
        else
        {
            hsm->Error( HSMFUI_ERROR_INVALID_STATE );
        }
    }
    else
    {
        hsm->Error( HSMFUI_ERROR_SET_ON_WRONG_STATUS );
    }
}

void hsmfui_Transition( Hsmfui * hsm, Hsmfui * state )
{
    if( hsm->Status == STATUS_ENTERED )
    {
        if( !hsm->IsOrth && isChild(hsm, state) )
        {
            /* TODO schedule transition */
        }
        else
        {
            hsm->Error( HSMFUI_ERROR_INVALID_STATE );
        }
    }
    else
    {
        hsm->Error( HSMFUI_ERROR_TRANSITION_ON_WRONG_STATUS );
    }
}

/***********************************************************/

static void doForActive( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) )
{
    int i;
    Hsmfui * child;

    f( hsm );

    if( hsm->IsOrth )
    {
        for(i = 0; i < hsm->NChildren; i++)
        {
            child = hsm->Children[i];
            doForActive( child, f );
        }
    }
    else 
    {
        child = hsm->State;
        if( child != NULL ) doForActive( child, f );
    }
}

static void doForAll( Hsmfui * hsm, void ( * f ) ( Hsmfui * ) )
{
    int i;
    Hsmfui * child;

    f( hsm );

    for(i = 0; i < hsm->NChildren; i++)
    {
        child = hsm->Children[i];
        doForAll( child, f );
    }
}

/***********************************************************/

static void setChildrensParent( Hsmfui * hsm )
{
    int i;
    Hsmfui * child;
    for( i = 0; i < hsm->NChildren; i++ )
    {
        child = hsm->Children[i];
        child->Parent = hsm;
    }
}

static void activateFirstChild( Hsmfui * hsm )
{
    if( !hsm->IsOrth && hsm->NChildren > 0 ) hsm->State = hsm->Children[0];
}

static void setErrorHandler( Hsmfui * hsm )
{
    if( hsm->Error == NULL )
    {
        if(hsm->Parent != NULL && hsm->Parent->Error!= NULL) 
        {
            hsm->Error = hsm->Parent->Error;
        }
        else
        {
            hsm->Error = defaultErrorHandler;
        }
    }
}

static void doInit( Hsmfui * hsm )
{
    void (*handler)(void);

    if( hsm->Status == STATUS_NULL )
    {
        hsm->Status = STATUS_INITIALISED;
        handler = hsm->Init;
        if( handler != NULL) handler();
    }
    else
    {
        hsm->Error( HSMFUI_ERROR_DOUBLE_INITIALISATION );
    }
}

static void doEnt( Hsmfui * hsm )
{
    void (*handler)(void);
    
    if( hsm->Status == STATUS_INITIALISED )
    {
        hsm->Status = STATUS_ENTERED;
        handler = hsm->Ent;
        if( handler != NULL) handler();
    }
    else
    {
        hsm->Error( HSMFUI_ERROR_DOUBLE_ENTRY );
    }
}

static void doAct( Hsmfui * hsm )
{
    void (*handler)(void);

    if( hsm->Status == STATUS_ENTERED )
    {
        handler = hsm->Act;
        if( handler != NULL) handler();
    }
    else
    {
        hsm->Error( HSMFUI_ERROR_ACT_WITHOUT_ENT );
    }
}

static void doExi( Hsmfui * hsm )
{
    void (*handler)(void);
    
    if( hsm->Status == STATUS_ENTERED )
    {
        hsm->Status = STATUS_INITIALISED;
        handler = hsm->Exi;
        if( handler != NULL) handler();
    }
    else
    {
        hsm->Error( HSMFUI_ERROR_DOUBLE_EXIT );
    }
}

static int isChild( Hsmfui * father, Hsmfui * child)
{
    int i;

    for(i = 0; i < father->NChildren; i++)
    {
        if( child == father->Children[i] ) return 1;
    }
    return 0;
}

static int countSubstate( Hsmfui * hsm, Hsmfui * state)
{
    int i;
    Hsmfui * child;
    int sum = 0;

    if ( hsm == state ) sum++;

    for(i = 0; i < hsm->NChildren; i++)
    {
        child = hsm->Children[i];
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
        for(i = 0; i < subSm->NChildren; i++)
        {
            child = subSm->Children[i];
            if( hasDuplicate( top, child ) )
            {
                result = 1;
                break;    /* prevent infinite loop */
            }
        }
    }

    #ifdef DEBUG
    /* printf("%s(%s,%s) = %d\n", __FUNCTION__, top->Name, subSm->Name, result); */
    #endif
    
    return result;
}
