VAR x = 3

{
    - x == 1: one
    - x == 2: two
    - else: other
}

{
    - x == 1: one
    - x == 2: two
    - other
}

{ x == 4:
  - The main clause
  - else: other
}

{ x == 4:
  The main clause
- else:
  other
}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"TEST_CURRENT_STORY_TEXT": "other"},
	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"TEST_CURRENT_STORY_TEXT": "other"},
	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"TEST_CURRENT_STORY_TEXT": "other"},
	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"TEST_CURRENT_STORY_TEXT": "other"}
]
INK_TEST_END
*/