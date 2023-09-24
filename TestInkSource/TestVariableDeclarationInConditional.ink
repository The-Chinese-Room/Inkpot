
VAR x = 0
{true:
    - ~ x = 5
}
{x}
                

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_STORY_CONTINUE": "5\n"}
]
INK_TEST_END
*/