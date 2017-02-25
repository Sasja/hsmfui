#include <stdio.h>
#include "Hsmfui.h"

#define ALL_SM \
X(sm                    ) \
X(    one               ) \
X(    two               ) \
X(        two_a         ) \
X(            two_a_1   ) \
X(            two_a_2   ) \
X(            two_a_3   ) \
X(        two_b         ) \
X(    three             ) \
X(       three_a        ) \
X(            three_a_1 ) \
X(            three_a_2 ) \
X(       three_b        ) \
X(       three_c        )


LEAF(three_c)
LEAF(three_b)

LEAF(three_a_2)
LEAF(three_a_1)

NODE_START(three_a)
    CHILD(three_a_1)
    CHILD(three_a_2)
NODE_STOP(three_a)

ORTH_START(three)
    CHILD(three_a)
    CHILD(three_b)
    CHILD(three_c)
ORTH_STOP(three)

LEAF(two_b)

LEAF(two_a_3)
LEAF(two_a_2)
LEAF(two_a_1)

NODE_START(two_a)
    CHILD(two_a_1)
    CHILD(two_a_2)
    CHILD(two_a_3)
NODE_STOP(two_a)

NODE_START(two)
    CHILD(two_a)
    CHILD(two_b)
NODE_STOP(two)

LEAF(one)

NODE_START(sm)
    CHILD(one)
    CHILD(two)
    CHILD(three)
NODE_STOP(sm)


/* generate _Init for all states */

#define X(s) \
    static void s##_Init( void ) \
    { \
        printf("%s\n",__FUNCTION__); \
    }
ALL_SM
#undef X

/***********************************************************/

static void attachHandlers( void );

int main( void )
{
    int i;

    attachHandlers();
    hsmfui_Init( &sm );
    for(i=1; i<100; i++)
    {
        hsmfui_Act( &sm );
    }
    hsmfui_Exi( &sm );

    return 0;
}

static void attachHandlers( void )
{
    /* attach all generated _Init functions to all nodes */
    #define X(s) s.Init = s##_Init;
    ALL_SM
    #undef X
}

