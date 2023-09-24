
-> knot.stitch

== knot (y) ==
~temp x = 5
-> END

= stitch
{x} {y}
-> END
			

/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 2},
	{"TEST_WARNING_EQUAL": 0},
    {"TEST_ERROR_TEXT":"Unresolved variable: x"},
    {"TEST_ERROR_TEXT":"Unresolved variable: y"},

]
INK_TEST_END
*/
