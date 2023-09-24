
VAR global_var = 5

~ pass_divert(-> knot_name)
{variable_param_test(10)}

=== function aTarget() ===
   ~ return true

=== function pass_divert(aTarget) ===
    Should be a divert target, but is a read count:- {aTarget}

=== function variable_param_test(global_var) ===
    ~ return global_var

=== knot_name ===
    -> END

/*
INK_TEST_START
[
	{"TEST_ERROR_EQUAL": 2},
	{"TEST_WARNING_EQUAL": 1}
]
INK_TEST_END
*/