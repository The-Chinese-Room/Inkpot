
EXTERNAL TRUE ()

Phrase 1 
{ TRUE ():

	Phrase 2
}
-> END 

=== function TRUE ()
	~ return true

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Phrase 1\nPhrase 2\n"},
]
INK_TEST_END
*/