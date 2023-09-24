
- start
 {
    - true: * [go to a stitch] -> a_stitch
 }
- gather should be seen
-> DONE

= a_stitch
    result
    -> END
                

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "start\ngather should be seen"},
    {"TEST_CHOICE_COUNT": 1},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_STORY_CONTINUE": "result"}
]
INK_TEST_END
*/