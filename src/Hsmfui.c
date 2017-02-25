#include "Hsmfui.h"
#include <stdio.h>

void hsmfui_Init( Hsmfui * hsm )
{
	int i;
	Hsmfui * child;

	#ifdef DEBUG
	/* printf("%s(&%s)\n", __FUNCTION__, hsm->name); */
	#endif

	if( hsm->Init != NULL) hsm->Init();

	for(i=0; i<hsm->nChildren; i++)
	{
		child = hsm->children[i];
		child->parent = hsm;
		hsmfui_Init( child );
	}
}

void hsmfui_Act( Hsmfui * hsm )
{
}

void hsmfui_Exi( Hsmfui * hsm )
{
}


int8_t AverageThreeBytes(int8_t a, int8_t b, int8_t c)
{
return (int8_t)(((int16_t)a + (int16_t)b + (int16_t)c) / 3);
}
