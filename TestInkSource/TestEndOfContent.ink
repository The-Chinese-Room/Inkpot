/*

1) Test end of content with plain Starting
2) Test end of content with ->END
3) Should have runtime error due to running out of content (needs a -> END)
4) Should have warning that there's no "-> END"
5) Return statements can only be used in knots that are declared as functions
6) Functions may not contain diverts


INK_TEST_STORY_START
[
    {"COMPILE_STORY_STRING": "Hello world"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    
    {"COMPILE_STORY_STRING": "== test ==\nContent\n-> END"},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    
    {"COMPILE_STORY_STRING": "== test ==\nContent"},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 1},
	
	{"COMPILE_STRING": "== test ==\nContent"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 1},
	
	{"COMPILE_STRING": "== test ==\n~return"},
    {"TEST_ERROR_EQUAL": 1},
    
    {"COMPILE_STRING": "== function test ==\n-> END"},
    {"TEST_ERROR_EQUAL": 1},
]
INK_TEST_END
*/