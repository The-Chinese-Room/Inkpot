
* { not test } visible choice
* { test } visible choice

== test ==
-> END
                
/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CHOICE_COUNT": 1},
    {"TEST_CHOICE_TEXT": [0, "visible choice"]},
]
INK_TEST_END
*/