VAR x = -> here
-> there ->
-> there ->
-> END

== there ==
-> x ->
~x = -> herehere
->->

== here ==
Here.
->->

== herehere ==
* [Where?]
Here, here!
->->


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "Here.\n"},
	{"TEST_CHOICE_COUNT": 1 },
	{"EXECUTE_SAVE_JSON_STATE": "JSON"},
    {"EXECUTE_RECOMPILE_STORY": ""},
	{"EXECUTE_LOAD_JSON_STATE": "JSON"},
	{"EXECUTE_STORY_CHOICE": 0 },
	{"EXECUTE_SAVE_JSON_STATE": "JSON"},
    {"EXECUTE_RECOMPILE_STORY": ""},
	{"EXECUTE_LOAD_JSON_STATE": "JSON"},
	{"EXECUTE_STORY_CONTINUE": "Here, here!\n"},
]
INK_TEST_END
*/