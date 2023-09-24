
<- choices
{ CHOICE_COUNT() }

= end
-> END

= choices
* one -> end
* two -> end

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_STORY_CONTINUE": "2"}
]
INK_TEST_END
*/