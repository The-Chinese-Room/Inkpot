
=== knot1
knot 1 line 1
knot 1 line 2
-> END 

=== knot2
knot 2 line 1
knot 2 line 2
-> END 


/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},
    
    {"EXECUTE_SWITCH_FLOW": "First"},
    {"EXECUTE_CHOOSE_STRING_PATH": "knot1"},
    {"EXECUTE_STORY_CONTINUE": "knot 1 line 1\n"},

    {"EXECUTE_SWITCH_FLOW": "Second"},
    {"EXECUTE_CHOOSE_STRING_PATH": "knot2"},
    {"EXECUTE_STORY_CONTINUE": "knot 2 line 1\n"},


    {"EXECUTE_SWITCH_FLOW": "First"},
    {"EXECUTE_STORY_CONTINUE": "knot 1 line 2\n"},

    {"EXECUTE_SWITCH_FLOW": "Second"},
    {"EXECUTE_STORY_CONTINUE": "knot 2 line 2\n"},
]
INK_TEST_END
*/