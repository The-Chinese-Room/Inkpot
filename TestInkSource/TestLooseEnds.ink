No loose ends in main content.

== knot1 ==
* loose end choice
* loose end
	on second line of choice

== knot2 ==
* A
* B
TODO: Fix loose ends but don't warn

== knot3 ==
Loose end when there's no weave

== knot4 ==
{true:
    {false:
        Ignore loose end when there's a divert
        in a conditional.
        -> knot4
	}
}
        
/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 3},
    {"TEST_WARNING_TEXT": "line 4: Apparent loose end"},
    {"TEST_WARNING_TEXT": "line 6: Apparent loose end"},
    {"TEST_WARNING_TEXT": "line 14: Apparent loose end"}
]
INK_TEST_END
*/