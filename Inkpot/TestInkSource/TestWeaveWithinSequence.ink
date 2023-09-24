
{ shuffle:
-   * choice
    nextline
    -> END
}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	
	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"TEST_CHOICE_COUNT": 1},
	{"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "choice\nnextline\n"}
]
INK_TEST_END
*/
