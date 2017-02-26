#include "unity.h"
#include "Hsmfui.h"

#define ALL_SM \
X( sm                    ) \
X(     one               ) \
X(     two               ) \
X(         two_a         ) \
X(             two_a_1   ) \
X(             two_a_2   ) \
X(             two_a_3   ) \
X(         two_b         ) \
X(     three             ) \
X(        three_a        ) \
X(             three_a_1 ) \
X(             three_a_2 ) \
X(        three_b        ) \
X(        three_c        )

#define NUMBER_OF_STATES 14

#define HSM_DEFINITION  \
LEAF(three_c)           \
LEAF(three_b)           \
                        \
LEAF(three_a_2)         \
LEAF(three_a_1)         \
                        \
NODE_START(three_a)     \
    CHILD(three_a_1)    \
    CHILD(three_a_2)    \
NODE_STOP(three_a)      \
                        \
ORTH_START(three)       \
    CHILD(three_a)      \
    CHILD(three_b)      \
    CHILD(three_c)      \
ORTH_STOP(three)        \
                        \
LEAF(two_b)             \
                        \
LEAF(two_a_3)           \
LEAF(two_a_2)           \
LEAF(two_a_1)           \
                        \
NODE_START(two_a)       \
    CHILD(two_a_1)      \
    CHILD(two_a_2)      \
    CHILD(two_a_3)      \
NODE_STOP(two_a)        \
                        \
NODE_START(two)         \
    CHILD(two_a)        \
    CHILD(two_b)        \
NODE_STOP(two)          \
                        \
LEAF(one)               \
                        \
NODE_START(sm)          \
    CHILD(one)          \
    CHILD(two)          \
    CHILD(three)        \
NODE_STOP(sm)           


enum allStates {
    #define X(s) s##_number,
    ALL_SM
    #undef X
};

static char log_Init[NUMBER_OF_STATES + 1];
static char log_Ent[NUMBER_OF_STATES + 1];
static char log_Act[NUMBER_OF_STATES + 1];
static char log_Exi[NUMBER_OF_STATES + 1];
static enum hsmfui_error log_Error_value;
static int log_Error_count;

#define X(s) \
    static void s##_Init_log( void ) \
    { \
        log_Init[s##_number]++; \
    } \
    static void s##_Ent_log( void ) \
    { \
        log_Ent[s##_number]++; \
    } \
    static void s##_Act_log( void ) \
    { \
        log_Act[s##_number]++; \
    } \
    static void s##_Exi_log( void ) \
    { \
        log_Exi[s##_number]++; \
    }
ALL_SM
#undef X

static void clearLogs( void )
{
    int i;
    for(i=0; i<NUMBER_OF_STATES; i++)
    {
        log_Init[i] = '0';
        log_Ent[i] = '0';
        log_Act[i] = '0';
        log_Exi[i] = '0';
    }
    log_Init[NUMBER_OF_STATES] = '\0';
    log_Ent[NUMBER_OF_STATES]  = '\0';
    log_Act[NUMBER_OF_STATES]  = '\0';
    log_Exi[NUMBER_OF_STATES]  = '\0';

    log_Error_count = 0;
    log_Error_value = HSMFUI_ERROR_NONE;
}

static void error_log( enum hsmfui_error error )
{
    log_Error_count++;
    log_Error_value = error;
}

static void error_ignore( enum hsmfui_error error )
{
    /* scratch your nose */
}

/**********************************************************/

void test_Init_should_propagate( void )
{
    HSM_DEFINITION

    clearLogs();
    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Init);

    /* plug counter into all init handler pointers */
    #define X(s) s.Init = s##_Init_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );

    TEST_ASSERT_EQUAL_STRING("11111111111111", log_Init);
}


void test_Init_should_set_all_parentpointers( void )
{
    HSM_DEFINITION
    
    #define X(s) TEST_ASSERT_EQUAL(NULL, s.parent);
    ALL_SM
    #undef X

    hsmfui_Init( &sm );

    TEST_ASSERT_EQUAL(NULL,     sm.parent);

    TEST_ASSERT_EQUAL(&sm,      one.parent);
    TEST_ASSERT_EQUAL(&sm,      two.parent);
    TEST_ASSERT_EQUAL(&sm,      three.parent);

    TEST_ASSERT_EQUAL(&two,     two_a.parent);
    TEST_ASSERT_EQUAL(&two,     two_b.parent);

    TEST_ASSERT_EQUAL(&three,   three_a.parent);
    TEST_ASSERT_EQUAL(&three,   three_b.parent);
    TEST_ASSERT_EQUAL(&three,   three_c.parent);

    TEST_ASSERT_EQUAL(&two_a,   two_a_1.parent);
    TEST_ASSERT_EQUAL(&two_a,   two_a_2.parent);
    TEST_ASSERT_EQUAL(&two_a,   two_a_3.parent);

    TEST_ASSERT_EQUAL(&three_a, three_a_1.parent);
    TEST_ASSERT_EQUAL(&three_a, three_a_2.parent);

}

void test_Init_should_set_all_states( void )
{
    HSM_DEFINITION

    #define X(s) TEST_ASSERT_EQUAL(NULL, s.state);
    ALL_SM
    #undef X

    hsmfui_Init( &sm );

    TEST_ASSERT_EQUAL(&one, sm.state);

    TEST_ASSERT_EQUAL(NULL,       one.state);
    TEST_ASSERT_EQUAL(&two_a,     two.state);
    TEST_ASSERT_EQUAL(NULL,       three.state); /* orthogonal node */

    TEST_ASSERT_EQUAL(&two_a_1,   two_a.state);
    TEST_ASSERT_EQUAL(NULL,       two_b.state);

    TEST_ASSERT_EQUAL(&three_a_1, three_a.state);
    TEST_ASSERT_EQUAL(NULL,       three_b.state);
    TEST_ASSERT_EQUAL(NULL,       three_c.state);

    TEST_ASSERT_EQUAL(NULL,       three_a_1.state);
    TEST_ASSERT_EQUAL(NULL,       three_a_2.state);

}

void test_Act_should_propagate( void )
{
    HSM_DEFINITION

    clearLogs();

    #define X(s) s.Act = s##_Act_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );

    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Act);

    hsmfui_Act( &sm );
    TEST_ASSERT_EQUAL_STRING("11000000000000", log_Act);

    sm.state = &two;
    hsmfui_Act( &sm );
    TEST_ASSERT_EQUAL_STRING("21111000000000", log_Act);

    sm.state = &three;  /* orthogonal node! */
    hsmfui_Act( &sm );
    TEST_ASSERT_EQUAL_STRING("31111000111011", log_Act);

    three_a.state = &three_a_2;
    hsmfui_Act( &sm );
    TEST_ASSERT_EQUAL_STRING("41111000221122", log_Act);
}

void test_Ent_should_propagate_A( void )
{
    HSM_DEFINITION

    clearLogs();
    #define X(s) s.Ent = s##_Ent_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Ent);

    hsmfui_Ent( &sm );
    TEST_ASSERT_EQUAL_STRING("11000000000000", log_Ent);
}

void test_Ent_should_propagate_B( void )
{
    HSM_DEFINITION

    clearLogs();
    #define X(s) s.Ent = s##_Ent_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Ent);

    sm.state = &two;
    hsmfui_Ent( &sm );
    TEST_ASSERT_EQUAL_STRING("10111000000000", log_Ent);
}

void test_Ent_should_propagate_C( void )
{
    HSM_DEFINITION

    clearLogs();
    #define X(s) s.Ent = s##_Ent_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Ent);

    sm.state = &three;  /* orthogonal node! */
    hsmfui_Ent( &sm );
    TEST_ASSERT_EQUAL_STRING("10000000111011", log_Ent);
}

void test_Exi_should_propagate_A( void )
{
    HSM_DEFINITION

    clearLogs();
    #define X(s) s.Exi = s##_Exi_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Exi);

    hsmfui_Exi( &sm );
    TEST_ASSERT_EQUAL_STRING("11000000000000", log_Exi);
}

void test_Exi_should_propagate_B( void )
{
    HSM_DEFINITION

    clearLogs();
    #define X(s) s.Exi = s##_Exi_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Exi);

    sm.state = &two;
    hsmfui_Exi( &sm );
    TEST_ASSERT_EQUAL_STRING("10111000000000", log_Exi);
}

void test_Exi_should_propagate_C( void )
{
    HSM_DEFINITION

    clearLogs();
    #define X(s) s.Exi = s##_Exi_log;
    ALL_SM
    #undef X

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL_STRING("00000000000000", log_Exi);

    sm.state = &three;  /* orthogonal node! */
    hsmfui_Exi( &sm );
    TEST_ASSERT_EQUAL_STRING("10000000111011", log_Exi);
}

void test_Init_should_normally_not_trigger_error( void )
{
    HSM_DEFINITION

    sm.Error = error_log;

    clearLogs();

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL(0, log_Error_count);
}

void test_Init_should_report_duplicate_states( void )
{
    /*
    sm
        one         <--
        two
            a
            one     <--
    */

    LEAF(one)
    LEAF(a)

    NODE_START(two)
        CHILD(a)
        CHILD(one)
    NODE_STOP(two)

    /* LEAF(one) allready defined */

    NODE_START(sm)
        CHILD(one)
        CHILD(two)
    NODE_STOP(sm)

    clearLogs();

    sm.Error = error_log;

    hsmfui_Init( &sm );
    TEST_ASSERT_EQUAL(HSMFUI_ERROR_DUPLICATE_STATE, log_Error_value);
    TEST_ASSERT_EQUAL(0, log_Error_count);
}

void test_getErrorString( void )
{
    #define X(error) TEST_ASSERT_EQUAL_STRING(#error, hsmfui_getErrorString(error));
    HSMFUI_ERRORS
    #undef X
}

int main( void )
{
    UNITY_BEGIN();

    RUN_TEST(test_Init_should_propagate);
    RUN_TEST(test_Init_should_set_all_parentpointers);
    RUN_TEST(test_Init_should_set_all_states);

    RUN_TEST(test_Ent_should_propagate_A);
    RUN_TEST(test_Ent_should_propagate_B);
    RUN_TEST(test_Ent_should_propagate_C);

    RUN_TEST(test_Act_should_propagate);

    RUN_TEST(test_Exi_should_propagate_A);
    RUN_TEST(test_Exi_should_propagate_B);
    RUN_TEST(test_Exi_should_propagate_C);

    RUN_TEST(test_Init_should_normally_not_trigger_error);
    RUN_TEST(test_Init_should_report_duplicate_states);

    RUN_TEST(test_getErrorString);

    return UNITY_END();
}
