
Top level content
* choice

== somewhere ==
= here
-> DONE

== function test ==
~ return -> somewhere.here

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "-1"},
    {"EXECUTE_INK_FUNCTION": {
        "FUNCTION_NAME" : "test",
        "ARGS" : [],
        "FUNCTION_OUTPUT": "somewhere.here"
    }}

]
INK_TEST_END
*/
