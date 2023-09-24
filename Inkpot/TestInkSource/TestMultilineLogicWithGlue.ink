
{true:
    a 
} <> b


{true:
    a 
} <> { true: 
    b 
}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "a b\na b\n"}    
]
INK_TEST_END
*/