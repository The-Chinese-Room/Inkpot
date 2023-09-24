
VAR x = 5
~ x += one()
    
=== function one()
~ return 1


/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0}
]
INK_TEST_END
*/