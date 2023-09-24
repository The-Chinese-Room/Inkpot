
- first gather
    * [option 1]
    * [option 2]
- the main gather
{false:
    * unreachable option -> END
}
- bottom gather

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "first gather\n"},
    {"TEST_CHOICE_COUNT": 2},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "the main gather\nbottom gather\n"},
    {"TEST_CHOICE_COUNT": 0}
]
INK_TEST_END
*/