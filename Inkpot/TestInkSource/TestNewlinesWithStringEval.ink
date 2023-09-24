
A
~temp someTemp = string()
B

A 
{string()}
B

=== function string()    
    ~ return "{3}"
}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "A\nB\nA\n3\nB\n"},
]
INK_TEST_END
*/