
-> tunnel1 ->
The End.
-> END

== tunnel1 ==
Hello...
-> tunnel2 ->->

== tunnel2 ==
...world.
->->


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Hello...\n...world.\nThe End.\n"}
]
INK_TEST_END
*/