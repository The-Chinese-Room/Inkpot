
LIST list = l, m = 5, n
{LIST_VALUE(l)}

{list(1)}

~ temp t = list()
~ t += n
{t}
~ t = LIST_ALL(t)
~ t -= n
{t}
~ t = LIST_INVERT(t)
{t}


/*
INK_TEST_STORY_START
[
    {"TEST_ERROR_EQUAL": 0},
    {"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "1\nl\nn\nl, m\nn\n"},
]
INK_TEST_END
*/