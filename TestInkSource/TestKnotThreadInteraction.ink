
-> knot
=== knot
    <- threadB
    -> tunnel ->
    THE END
    -> END

=== tunnel
    - blah blah
    * wigwag
    - ->->

=== threadB
    *   option
    -   something
        -> DONE


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "blah blah\n"},
    {"TEST_CHOICE_COUNT": 2},
    {"TEST_CHOICE_TEXT": [0, "option"]},
    {"TEST_CHOICE_TEXT": [1, "wigwag"]},
    {"EXECUTE_STORY_CHOICE": 1},
    {"EXECUTE_STORY_CONTINUE": "wigwag\n"},
    {"EXECUTE_STORY_CONTINUE": "THE END\n"},
]
INK_TEST_END
*/