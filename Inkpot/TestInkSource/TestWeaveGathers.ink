
-
 * one
    * * two
   - - three
 *  four
   - - five
- six
                
/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
	{"TEST_CHOICE_COUNT": 2},
	{"TEST_CHOICE_TEXT": [0,"one"]},
	{"TEST_CHOICE_TEXT": [1,"four"]},
  {"EXECUTE_STORY_CHOICE": 0},
  {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
  {"TEST_CHOICE_COUNT": 1},
	{"TEST_CHOICE_TEXT": [0,"two"]},
  {"EXECUTE_STORY_CHOICE": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "two\nthree\nsix\n"},
]
INK_TEST_END
*/