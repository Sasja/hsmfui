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

static int countSubstate( Hsmfui * hsm, Hsmfui * state );
static int hasDuplicate(  Hsmfui * top, Hsmfui * subSm );

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
        hsm->Error( HSMFUI_ERROR_DUPLICATE_STATE );
    }
    else
    {
        doForAll( hsm, setChildrensParent );
        doForAll( hsm, activateFirstChild );
        doForAll( hsm, callInit );
    }
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
		if( sum > 1 ) break;	/* prevent infinite loop */
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
                break;	/* prevent infinite loop */
            }
        }
    }

    #ifdef DEBUG
    /* printf("%s(%s,%s) = %d\n", __FUNCTION__, top->name, subSm->name, result); */
    #endif
    
	return result;
}
