
-> empty_world
=== empty_world ===
    {TURNS_SINCE(-> then)} = -1
    * (then) stuff
        {TURNS_SINCE(-> then)} = 0
        * * (next) more stuff
            {TURNS_SINCE(-> then)} = 1
        -> DONE

/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},

    {"EXECUTE_CONTINUE_MAXIMALLY": "-1 = -1\n"},
    
    {"TEST_CHOICE_COUNT": 1},
    {"EXECUTE_STORY_CHOICE": 0},

    {"EXECUTE_CONTINUE_MAXIMALLY": "stuff\n0 = 0\n"},

    {"TEST_CHOICE_COUNT": 1},
    {"EXECUTE_STORY_CHOICE": 0},


    {"EXECUTE_CONTINUE_MAXIMALLY": "more stuff\n1 = 1\n"}
]
INK_TEST_END
*/