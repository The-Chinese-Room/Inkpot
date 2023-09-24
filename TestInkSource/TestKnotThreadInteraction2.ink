
-> knot
=== knot
    <- threadA
    When should this get printed?
    -> DONE

=== threadA
    -> tunnel ->
    Finishing thread.
    -> DONE

=== tunnel
    -   I'm in a tunnel
    *   I'm an option
    -   ->->

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "I'm in a tunnel\nWhen should this get printed?\n"},
    {"TEST_CHOICE_COUNT": 1},
    {"TEST_CHOICE_TEXT": [0, "I'm an option"]},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "I'm an option\nFinishing thread.\n"},
]
INK_TEST_END
*/