#include "Hsmfui.h"
#include <stdio.h>

void hsmfui_Init( Hsmfui * hsm )
{
	int i;
	Hsmfui * child;

	if( hsm->Init != NULL) hsm->Init();

    if( !hsm->isOrth && hsm->nChildren > 0) hsm->state = hsm->children[0];

	for(i=0; i<hsm->nChildren; i++)
	{
		child = hsm->children[i];
		child->parent = hsm;
		hsmfui_Init( child );
	}
}

void hsmfui_Act( Hsmfui * hsm )
{
    int i;
	Hsmfui * child;

	if( hsm->Act != NULL) hsm->Act();

    if( hsm->isOrth )
    {
        for(i=0; i<hsm->nChildren; i++)
        {
            child = hsm->children[i];
            hsmfui_Act( child );
        }
    }
    else 
    {
        if( hsm->state != NULL ) hsmfui_Act( hsm->state );
    }
}

void hsmfui_Exi( Hsmfui * hsm )
{
}

