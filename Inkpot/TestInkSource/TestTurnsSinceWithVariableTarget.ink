
-> start

=== start ===
    {beats(-> start)}
    {beats(-> start)}
    *   [Choice]  -> next
= next
    {beats(-> start)}
    -> END

=== function beats(x) ===
    ~ return TURNS_SINCE(x)

/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},

    {"EXECUTE_CONTINUE_MAXIMALLY": "0\n0\n"},
    {"EXECUTE_STORY_CHOICE": 0},
    
    {"EXECUTE_CONTINUE_MAXIMALLY": "1\n"}
]
INK_TEST_END
*/