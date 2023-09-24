
{ TURNS_SINCE(-> test) }
~ test()
{ TURNS_SINCE(-> test) }
* [choice 1]
- { TURNS_SINCE(-> test) }
* [choice 2]
- { TURNS_SINCE(-> test) }

== function test ==
~ return

/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},

    {"EXECUTE_CONTINUE_MAXIMALLY": "-1\n0\n"},
    
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "1\n"},

    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "2\n"},
]
INK_TEST_END
*/