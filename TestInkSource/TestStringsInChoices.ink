
* \ {"test1"} ["test2 {"test3"}"] {"test4"}
-> DONE


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CHOICE_COUNT": 1},
	{"TEST_CHOICE_TEXT": [0,"test1 \"test2 test3\""]},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_STORY_CONTINUE": "test1 test4\n"}
]
INK_TEST_END
*/