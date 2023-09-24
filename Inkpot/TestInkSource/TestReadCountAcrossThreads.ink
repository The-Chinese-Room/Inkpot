
    -> top

= top
    {top}
    <- aside
    {top}
    -> DONE

= aside
    * {false} DONE
	- -> DONE

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "1\n1\n"}
]
INK_TEST_END
*/