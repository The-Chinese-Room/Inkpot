
* -> hello
Unreachable
- (hello) 1
* ->
   - - 2
- 3
-> END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "1\n2\n3\n"}
]
INK_TEST_END
*/