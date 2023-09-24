
CONST pi = 3.1415
CONST pi = 3.1415

CONST x = "Hello"
CONST x = "World"

CONST y = 3
CONST y = 3.0

CONST z = -> somewhere
CONST z = -> elsewhere

== somewhere ==
-> DONE

== elsewhere ==
-> DONE


/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 3},
	{"TEST_WARNING_EQUAL": 0},
    {"TEST_ERROR_TEXT":"'x' has been redefined"},
    {"TEST_ERROR_TEXT":"'y' has been redefined"},
    {"TEST_ERROR_TEXT":"'z' has been redefined"}
]
INK_TEST_END
*/
