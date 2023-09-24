
LIST list = (a), b, (c), d, e
LIST list2 = x, (y), z
{list + list2}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "a, y, c\n"}
]
INK_TEST_END
*/