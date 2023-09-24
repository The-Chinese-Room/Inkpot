
{RunAThing()}

== function RunAThing ==
The first line.
The second line.

== SomewhereElse ==
{"somewhere else"}
->END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "The first line.\n"},
    {"EXECUTE_CHOOSE_STRING_PATH": "SomewhereElse"},
    {"EXECUTE_CONTINUE_MAXIMALLY": "somewhere else\n"}    
]
INK_TEST_END
*/