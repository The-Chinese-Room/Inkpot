
LIST list = a, (b), c, (d), e
{list}
{(a, c) + (b, e)}
{(a, b, c) ^ (c, b, e)}
{list ? (b, d, e)}
{list ? (d, b)}
{list !? (c)}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "b, d\na, b, c, e\nb, c\nfalse\ntrue\ntrue\n"}
]
INK_TEST_END
*/