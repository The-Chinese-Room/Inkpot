
Start
-> tunnel ->
End
-> END

== tunnel ==
In tunnel.
->->

=== function function_to_evaluate() ===
    { zero_equals_(1):
        ~ return "WRONG"
    - else:
        ~ return "RIGHT"
    }

=== function zero_equals_(k) ===
    ~ do_nothing(0)
    ~ return  (0 == k)

=== function do_nothing(k)
    ~ return 0


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "Start\n"},
    {"EXECUTE_STORY_CONTINUE": "In tunnel.\n"},
    {"EXECUTE_INK_FUNCTION": {
        "FUNCTION_NAME" : "function_to_evaluate",
        "ARGS" : [],
        "FUNCTION_OUTPUT": "RIGHT"
    }}

]
INK_TEST_END
*/
