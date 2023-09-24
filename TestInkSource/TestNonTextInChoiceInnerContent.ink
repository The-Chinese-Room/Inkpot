
-> knot
== knot
   *   option text[]. {true: Conditional bit.} -> next
   -> DONE

== next
    Next.
    -> DONE
               
/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
   {"EXECUTE_STORY_CONTINUE": "-1"},
   {"EXECUTE_STORY_CHOICE": 0},
   {"EXECUTE_STORY_CONTINUE": "option text. Conditional bit. Next.\n"},
]
INK_TEST_END
*/