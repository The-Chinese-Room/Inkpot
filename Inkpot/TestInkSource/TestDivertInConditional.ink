
=== intro
= top
    { main: -> done }
    -> END
= main
    -> top
= done
    -> END
                
/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": ""}
]
INK_TEST_END
*/