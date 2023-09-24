
-> test

== test ==
~ myFunc()
= function myBadInnerFunc
Not allowed!
~ return

== function myFunc ==
Hello world
* a choice
* another choice
-
-> myFunc
= testStitch
    This is a stitch
~ return


/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 7},
	{"TEST_WARNING_EQUAL": 0},
    {"TEST_ERROR_TEXT":"Return statements can only be used in knots that"},
    {"TEST_ERROR_TEXT":"Functions cannot be stitches"},
    {"TEST_ERROR_TEXT":"Functions may not contain stitches"},
    {"TEST_ERROR_TEXT":"Functions may not contain diverts"},
    {"TEST_ERROR_TEXT":"Functions may not contain choices"},
    {"TEST_ERROR_TEXT":"Functions may not contain choices"},
    {"TEST_ERROR_TEXT":"Return statements can only be used in knots that"}
]
INK_TEST_END
*/