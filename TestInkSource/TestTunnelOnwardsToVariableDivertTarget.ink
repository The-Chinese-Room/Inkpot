-> outer ->

== outer
This is outer
-> cut_to(-> the_esc)

=== cut_to(-> escape) 
    ->-> escape
    
== the_esc
This is the_esc
-> END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
	{"EXECUTE_CONTINUE_MAXIMALLY": "This is outer\nThis is the_esc\n"}
]
INK_TEST_END
*/