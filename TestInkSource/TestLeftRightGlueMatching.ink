
A line.
{ f():
    Another line.
}

== function f ==
{false:nothing}
~ return true

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "A line.\nAnother line.\n"}
]
INK_TEST_END
*/