
One
Two
Three

== function func1 ==
This is a function
~ return 5

== function func2 ==
This is a function without a return value
~ return

== function add(x,y) ==
x = {x}, y = {y}
~ return x + y

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},

    {"EXECUTE_INK_FUNCTION": {
        "FUNCTION_NAME" : "func1",
        "ARGS" : [],
        "FUNCTION_OUTPUT": "5"
    }},
    {"EXECUTE_STORY_CONTINUE": "One\n"},
    {"EXECUTE_INK_FUNCTION": {
        "FUNCTION_NAME" : "func2",
        "ARGS" : [],
        "FUNCTION_OUTPUT": ""
    }},
    {"EXECUTE_STORY_CONTINUE": "Two\n"},
    {"EXECUTE_INK_FUNCTION": {
        "FUNCTION_NAME" : "add",
        "ARGS" : [1, 2],
        "FUNCTION_OUTPUT": "3"
    }},
    {"EXECUTE_STORY_CONTINUE": "Three\n"},

]
INK_TEST_END
*/
