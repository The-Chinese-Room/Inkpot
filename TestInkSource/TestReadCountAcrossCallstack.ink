
-> first

== first ==
1) Seen first {first} times.
-> second ->
2) Seen first {first} times.
-> DONE

== second ==
In second.
->->

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "1) Seen first 1 times.\nIn second.\n2) Seen first 1 times.\n"}
]
INK_TEST_END
*/