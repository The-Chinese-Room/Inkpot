
* { true } { false } not displayed
* { true } { true }
  { true and true }  one
* { false } not displayed
* (name) { true } two
* { true }
  { true }
  three
* { true }
  four
                
/*
INK_TEST_STORY_START
[
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CHOICE_COUNT": 4},
	  {"TEST_CHOICE_TEXT": [0, "one"]},
	  {"TEST_CHOICE_TEXT": [1,"two"]},
	  {"TEST_CHOICE_TEXT": [2,"three"]},
	  {"TEST_CHOICE_TEXT": [3,"four"]}
]
INK_TEST_END
*/