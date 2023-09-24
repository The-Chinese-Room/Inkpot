
CONST CONST_STR = "ConstantString"
VAR varStr = CONST_STR
{varStr == CONST_STR:success}


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "success\n"}    
]
INK_TEST_END
*/