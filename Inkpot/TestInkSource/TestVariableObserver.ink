
VAR testVar = 5
VAR testVar2 = 10

Hello world!

~ testVar = 15
~ testVar2 = 100

Hello world 2!

* choice

    ~ testVar = 25
    ~ testVar2 = 200

    -> END


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},

    {"OBSERVE_VARIABLE": "testVar"},
    {"EXECUTE_CONTINUE_MAXIMALLY": "-1"},

	{"TEST_OBSERVED_VARIABLE": "15"},
    {"TEST_OBSERVER_CALL_COUNT": 1},
    {"TEST_CHOICE_COUNT": 1},
    
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_STORY_CONTINUE": "-1"},

    {"TEST_OBSERVED_VARIABLE": "25"},
    {"TEST_OBSERVER_CALL_COUNT": 2},
]
INK_TEST_END
*/