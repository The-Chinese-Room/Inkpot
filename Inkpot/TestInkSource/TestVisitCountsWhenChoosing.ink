
== TestKnot ==
this is a test
+ [Next] -> TestKnot2

== TestKnot2 ==
this is the end
-> END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},

    {"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot",
        "EXPECTED_COUNT": 0
    }},
    {"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot2",
        "EXPECTED_COUNT": 0
    }},

    {"EXECUTE_CHOOSE_STRING_PATH": "TestKnot"},
    {"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot",
        "EXPECTED_COUNT": 1
    }},
    {"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot2",
        "EXPECTED_COUNT": 0
    }},

	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot",
        "EXPECTED_COUNT": 1
    }},
    {"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot2",
        "EXPECTED_COUNT": 0
    }},

	{"EXECUTE_STORY_CHOICE": 0},
	{"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot",
        "EXPECTED_COUNT": 1
    }},
    {"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot2",
        "EXPECTED_COUNT": 0
    }},

	{"EXECUTE_STORY_CONTINUE": "-1"},
	{"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot",
        "EXPECTED_COUNT": 1
    }},
    {"TEST_PATH_VISIT_COUNT": {
        "PATH": "TestKnot2",
        "EXPECTED_COUNT": 1
    }},
]
INK_TEST_END
*/