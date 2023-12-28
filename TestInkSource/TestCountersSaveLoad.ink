-> one ->
-> two ->
-> two ->
{one},{two}
-> END

== one 
    One
    ->->

== two 
    Two
    ->->

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "One\n"},
    {"EXECUTE_STORY_CONTINUE": "Two\n"},
    {"EXECUTE_STORY_CONTINUE": "Two\n"},
	{"EXECUTE_SAVE_JSON_STATE": "JSON"},
    {"EXECUTE_RECOMPILE_STORY": ""},
	{"EXECUTE_LOAD_JSON_STATE": "JSON"},
    {"EXECUTE_STORY_CONTINUE": "1,2\n"}

]
INK_TEST_END
*/
