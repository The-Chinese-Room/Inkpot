EXTERNAL message(x)
EXTERNAL multiply(A,B)
EXTERNAL times(i,str)
{message("hello world")}
{multiply( 5.5, 3)}
{times(3,"knock ")}
->END
/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"BIND_EXTERNAL_FUNCTION":"message"},
	{"BIND_EXTERNAL_FUNCTION":"multiply"},
	{"BIND_EXTERNAL_FUNCTION":"times"},
    {"EXECUTE_CONTINUE_MAXIMALLY": "hello world\n16.5\nknock knock knock\n"},
]
INK_TEST_END
*/
