
VAR x = 5

{x}

* [choice]
-
{x}

* [choice]
-

{x}

* [choice]
-

{x}

-> DONE

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},

	{"EXECUTE_CONTINUE_MAXIMALLY": "5\n"},
    {"TEST_VARIABLE" : {
        "VARIABLE_NAME": "x",
        "VARIABLE_TYPE": "Int",
        "VARIABLE_VALUE": 5
    }},

    {"SET_VARIABLE" : {
        "VARIABLE_NAME": "x",
        "VARIABLE_TYPE": "Int",
        "VARIABLE_VALUE": 10
    }},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "10\n"},
    {"TEST_VARIABLE" : {
        "VARIABLE_NAME": "x",
        "VARIABLE_TYPE": "Int",
        "VARIABLE_VALUE": 10
    }},

    {"SET_VARIABLE" : {
        "VARIABLE_NAME": "x",
        "VARIABLE_TYPE": "Float",
        "VARIABLE_VALUE": 8.5
    }},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "8.5\n"},
    {"TEST_VARIABLE" : {
        "VARIABLE_NAME": "x",
        "VARIABLE_TYPE": "Float",
        "VARIABLE_VALUE": 8.5
    }},

    {"SET_VARIABLE" : {
        "VARIABLE_NAME": "x",
        "VARIABLE_TYPE": "String",
        "VARIABLE_VALUE": "a string"
    }},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "a string\n"},
    {"TEST_VARIABLE" : {
        "VARIABLE_NAME": "x",
        "VARIABLE_TYPE": "String",
        "VARIABLE_VALUE": "a string"
    }},

    {"TEST_VARIABLE" : {
        "VARIABLE_NAME": "z",
        "VARIABLE_TYPE": "null",
        "VARIABLE_VALUE": null
    }},
]
INK_TEST_END
*/