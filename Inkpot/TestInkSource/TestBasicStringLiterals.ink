
VAR x = "Hello world 1"
{x}
Hello {"world"} 2.


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "Hello world 1\nHello world 2.\n"}
]
INK_TEST_END
*/