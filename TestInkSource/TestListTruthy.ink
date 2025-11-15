LIST DoctorsInSurgery = (Adams), Bernard, Cartwright, Denver, Eamonn
VAR GoodDoctors = (Denver, Eamonn)
{DoctorsInSurgery}
DoctorsInSurgery is evaluating as {DoctorsInSurgery:true|false}
~DoctorsInSurgery -= Adams
{DoctorsInSurgery}
DoctorsInSurgery is evaluating as {DoctorsInSurgery:true|false}
{GoodDoctors}
GoodDoctors is evaluating as {GoodDoctors:true|false}

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Adams\nDoctorsInSurgery is evaluating as true\nDoctorsInSurgery is evaluating as false\nDenver, Eamonn\nGoodDoctors is evaluating as true"}
]
INK_TEST_END
*/
