
* {condFunc()} [choice 1]
* {condFunc()} [choice 2]
* {condFunc()} [choice 3]
* {condFunc()} [choice 4]


=== function condFunc() ===
{shuffle:
    - ~ return false
    - ~ return true
    - ~ return true
    - ~ return false
}


/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_STORY_CONTINUE": "-1"},
    {"TEST_CHOICE_COUNT": 2}
]
INK_TEST_END
*/