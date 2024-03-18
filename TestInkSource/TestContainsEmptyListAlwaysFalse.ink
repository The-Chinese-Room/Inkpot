LIST list = (a), b
{list ? ()}
{() ? ()}
{() ? list}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "false\nfalse\nfalse\n"}
]
INK_TEST_END
*/