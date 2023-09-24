
~ temp one = 1
* \ {one}
- End of choice 
    -> another
* (another) this [is] another
 -> DONE

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "-1"},
    {"TEST_CHOICE_COUNT": 1},
	{"TEST_CHOICE_TEXT": [0, "1"]},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "1\nEnd of choice\nthis another\n"},
    {"TEST_CHOICE_COUNT": 0}
]
INK_TEST_END
*/