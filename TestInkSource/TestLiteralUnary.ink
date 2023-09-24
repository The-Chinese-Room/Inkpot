
VAR negativeLiteral = -1
VAR negativeLiteral2 = not not false
VAR negativeLiteral3 = !(0)

{negativeLiteral}
{negativeLiteral2}
{negativeLiteral3}


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1\nfalse\ntrue\n"}
]
INK_TEST_END
*/