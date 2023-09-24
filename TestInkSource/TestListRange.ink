
LIST Food = Pizza, Pasta, Curry, Paella
LIST Currency = Pound, Euro, Dollar
LIST Numbers = One, Two, Three, Four, Five, Six, Seven

VAR all = ()
~ all = LIST_ALL(Food) + LIST_ALL(Currency)
{all}
{LIST_RANGE(all, 2, 3)}
{LIST_RANGE(LIST_ALL(Numbers), Two, Six)}
{LIST_RANGE((Pizza, Pasta), -1, 100)} // allow out of range

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Pound, Pizza, Euro, Pasta, Dollar, Curry, Paella\nEuro, Pasta, Dollar, Curry\nTwo, Three, Four, Five, Six\nPizza, Pasta"}
]
INK_TEST_END
*/