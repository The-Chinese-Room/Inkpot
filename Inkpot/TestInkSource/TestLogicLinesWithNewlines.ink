
~ func ()
text 2

~temp tempVar = func ()
text 2

== function func ()
	text1
	~ return true


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "text1\ntext 2\ntext1\ntext 2\n"}
]
INK_TEST_END
*/