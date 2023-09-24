/*

INK_TEST_STORY_START
[
    {"COMPILE_STORY_STRING": "{true}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "true\n"},
    
    {"COMPILE_STORY_STRING": "{true + 1}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "2\n"},

    {"COMPILE_STORY_STRING": "{2 + true}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "3\n"},
    
    {"COMPILE_STORY_STRING": "{false + false}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "0\n"},
    
    {"COMPILE_STORY_STRING": "{true + true}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "2\n"},
    
    {"COMPILE_STORY_STRING": "{true == 1}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "true\n"},
    
    {"COMPILE_STORY_STRING": "{not 1}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "false\n"},
    
    {"COMPILE_STORY_STRING": "{not true}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "false\n"},
    
    {"COMPILE_STORY_STRING": "{3 > 1}"},
    {"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "true\n"},
]
INK_TEST_END
*/