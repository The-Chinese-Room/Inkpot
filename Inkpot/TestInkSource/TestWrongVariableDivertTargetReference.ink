
-> go_to_broken(-> SOMEWHERE)

== go_to_broken(-> b)
 -> go_to(-> b) // INSTEAD OF: -> go_to(b)

== go_to(-> a)
  -> a

== SOMEWHERE ==
Should be able to get here!
-> DONE


/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 1},
	{"TEST_WARNING_EQUAL": 0}
]
INK_TEST_END
*/