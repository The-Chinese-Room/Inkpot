
INCLUDE Includes/test_included_file3.ink

This is the main file

-> knot_in_2
                

/*
Include is not supported at the moment.

INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "The value of a variable in test file 2 is 5.\nThis is the main file\nThe value when accessed from knot_in_2 is 5.\n"},
]
INK_TEST_END
*/