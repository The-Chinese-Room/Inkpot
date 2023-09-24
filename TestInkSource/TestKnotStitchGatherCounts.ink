
VAR knotCount = 0
VAR stitchCount = 0

-> gather_count_test ->

~ knotCount = 0
-> knot_count_test ->

~ knotCount = 0
-> knot_count_test ->

-> stitch_count_test ->

== gather_count_test ==
VAR gatherCount = 0
- (loop)
~ gatherCount++
{gatherCount} {loop}
{gatherCount<3:->loop}
->->

== knot_count_test ==
~ knotCount++
{knotCount} {knot_count_test}
{knotCount<3:->knot_count_test}
->->


== stitch_count_test ==
~ stitchCount = 0
-> stitch ->
~ stitchCount = 0
-> stitch ->
->->

= stitch
~ stitchCount++
{stitchCount} {stitch}
{stitchCount<3:->stitch}
->->

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "1 1\n2 2\n3 3\n1 1\n2 1\n3 1\n1 2\n2 2\n3 2\n1 1\n2 1\n3 1\n1 2\n2 2\n3 2"}
]
INK_TEST_END
*/