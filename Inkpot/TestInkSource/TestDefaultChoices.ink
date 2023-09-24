
 - (start)
 * [Choice 1]
 * [Choice 2]
 * {false} Impossible choice
 * -> default
 - After choice
 -> start

== default ==
This is default.
-> DONE

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	
    {"EXECUTE_STORY_CONTINUE": ""},
    {"TEST_CHOICE_COUNT": 2},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_STORY_CONTINUE": "After choice\n"},
    {"TEST_CHOICE_COUNT": 1},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "After choice\nThis is default.\n"}
]
INK_TEST_END
*/