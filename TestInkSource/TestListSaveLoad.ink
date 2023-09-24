
LIST l1 = (a), b, (c)
LIST l2 = (x), y, z

VAR t = ()
~ t = l1 + l2
{t}

== elsewhere ==
~ t += z
{t}
-> END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "a, x, c\n"},
	{"EXECUTE_SAVE_JSON_STATE": "JSON"},
    {"EXECUTE_RECOMPILE_STORY": ""},
	{"EXECUTE_LOAD_JSON_STATE": "JSON"},
    {"EXECUTE_CHOOSE_STRING_PATH": "elsewhere"},
    {"EXECUTE_CONTINUE_MAXIMALLY": "a, x, c, z\n"}
]
INK_TEST_END
*/