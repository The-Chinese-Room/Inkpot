
// Allowed to do this
~ myFunc()

// Not allowed to to this
~ aKnot()

// Not allowed to do this
-> myFunc

== function myFunc ==
This is a function.
~ return

== aKnot ==
This is a normal knot.
-> END

/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 2},
	{"TEST_WARNING_EQUAL": 0}
]
INK_TEST_END
*/