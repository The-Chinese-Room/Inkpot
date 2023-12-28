-> f ->
-> END

== f
Hello
* [Hello] Good morning, how are you?
    I'm fine, thank you. 
->->

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_STORY_CONTINUE": "Hello\n"},
	{"TEST_CHOICE_COUNT": 1 },
	{"EXECUTE_STORY_CHOICE": 0 },
	{"EXECUTE_STORY_CONTINUE": "Good morning, how are you?\n"},
	{"EXECUTE_SAVE_JSON_STATE": "JSON"},
    {"EXECUTE_RECOMPILE_STORY": ""},
	{"EXECUTE_LOAD_JSON_STATE": "JSON"},
	{"EXECUTE_STORY_CONTINUE": "I'm fine, thank you.\n"}
]
INK_TEST_END
*/