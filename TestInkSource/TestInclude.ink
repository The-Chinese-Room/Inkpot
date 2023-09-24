
INCLUDE Includes/test_included_file.ink
  INCLUDE Includes/test_included_file2.ink

This is the main file.
                

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
  {"EXECUTE_CONTINUE_MAXIMALLY": "This is include 1.\nThis is include 2.\nThis is the main file.\n"},
]
INK_TEST_END
*/