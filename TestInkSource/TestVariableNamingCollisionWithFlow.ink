
LIST someList = A, B

~temp heldItems = (A) 
{LIST_COUNT (heldItems)}

=== function heldItems ()
~ return (A)
        


/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 1},
	{"TEST_WARNING_EQUAL": 0},
    {"TEST_ERROR_TEXT": "name has already been used for a function"},
]
INK_TEST_END
*/