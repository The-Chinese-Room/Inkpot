
-> knot_with_options ->
Finished tunnel.

Starting thread.
<- thread_with_options
* E
-
Done.

== knot_with_options ==
* A
* B
-
->->

== thread_with_options ==
* C
* D
- -> DONE

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": ""},
    {"TEST_CHOICE_COUNT": 2},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CONTINUE_CONTAINS": "Finished tunnel"},
    {"TEST_CHOICE_COUNT": 3},
    {"EXECUTE_STORY_CHOICE": 2},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},
    {"TEST_CONTINUE_CONTAINS": "Done."}
]
INK_TEST_END
*/