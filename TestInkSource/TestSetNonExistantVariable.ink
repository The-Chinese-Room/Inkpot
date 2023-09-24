
VAR x = "world"
Hello {x}.


/*
INK_TEST_STORY_START
[
    {"EXPECTED_EXCEPTION": "Cannot assign to a variable"},
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_STORY_CONTINUE": "Hello world.\n"},
    {"SET_VARIABLE" : {
        "VARIABLE_NAME": "y",
        "VARIABLE_TYPE": "String",
        "VARIABLE_VALUE": "earth"
    }}
]
INK_TEST_END
*/