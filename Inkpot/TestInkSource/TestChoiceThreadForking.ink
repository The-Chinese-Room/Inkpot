
-> generate_choice(1) ->

== generate_choice(x) ==
{true:
    + A choice
        Vaue of local var is: {x}
        -> END
}
->->
/*

// Generate the choice with the forked thread
// Save/reload
// Load the choice, it should have its own thread still
// that still has the captured temp x
// Don't want warning:


INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"EXECUTE_SAVE_JSON_STATE": "JSON"},
    {"EXECUTE_RECOMPILE_STORY": ""},
	{"EXECUTE_LOAD_JSON_STATE": "JSON"},
	{"EXECUTE_STORY_CHOICE": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
	{"TEST_WARNING_EQUAL": 0}
]
INK_TEST_END
*/