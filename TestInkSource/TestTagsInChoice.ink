+ one #one [two #two] three #three -> END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "-1"},
    {"TEST_CURRENT_TAG_COUNT": 0},
    {"TEST_CHOICE_COUNT":1 },
    {"TEST_CHOICE_TAGS": {
        "CHOICE": 0,
        "TAGS": ["one","two"]
    }},
    {"EXECUTE_STORY_CHOICE": 0 },
    {"EXECUTE_STORY_CONTINUE": "one three"},
    {"TEST_CURRENT_TAGS": ["one","three"] }
]
INK_TEST_END
*/
