
* 'Hello {name()}[, your name is {name()}.'],' I said, knowing full well that his name was {name()}.
-> DONE

== function name ==
Joe


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CHOICE_TEXT": [0, "'Hello Joe, your name is Joe.'"]},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "'Hello Joe,' I said, knowing full well that his name was Joe.\n"},
]
INK_TEST_END
*/