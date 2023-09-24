
                    -> test
                    === test
                        * Hello[.], world.
                        -> END
                
/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "-1"},
    {"TEST_CHOICE_TEXT": [0, "Hello."]},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_STORY_CONTINUE": "Hello, world.\n"}
]
INK_TEST_END
*/