
-> opts
= opts
*   (firstOpt) [First choice]   ->  opts
*   {firstOpt} [Second choice]  ->  opts
* -> end

- (end)
    -> END

/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CHOICE_COUNT": 1},
	{"TEST_CHOICE_TEXT": [0, "First choice"]},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CHOICE_COUNT": 1},
    {"TEST_CHOICE_TEXT": [0, "Second choice"]},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
]
INK_TEST_END
*/