
A
{f():X}
C

=== function f()
{ true: 
    ~ return false
}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "A\nC\n"}
]
INK_TEST_END
*/