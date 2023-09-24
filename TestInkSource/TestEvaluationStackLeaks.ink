
{false:
    
- else: 
    else
}

{6:
- 5: five
- else: else
}

-> onceTest ->
-> onceTest ->

== onceTest ==
{once:
- hi
}
->->

/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "else\nelse\nhi\n"},
	{"TEST_STORY_EVALUATION_STACK_COUNT": 0}
]
INK_TEST_END
*/