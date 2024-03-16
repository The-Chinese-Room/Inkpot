
VAR x = 2 
# author: Joe
# title: My Great Story
This is the content

== knot ==
# knot tag
Knot content
# end of knot tag
-> END

= stitch
# stitch tag
Stitch content
# this tag is below some content so isn't included in the static tags for the stitch
-> END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},
    {"TEST_STORY_GLOBAL_TAGS": ["author: Joe", "title: My Great Story"]},	
    {"EXECUTE_STORY_CONTINUE": "This is the content\n"},
    {"TEST_CURRENT_TAGS": ["author: Joe", "title: My Great Story"]},
    {"TEST_TAG_FOR_PATH": {
        "PATH": "knot",
        "TAGS": ["knot tag"]
    }},
    {"TEST_TAG_FOR_PATH": {
        "PATH": "knot.stitch",
        "TAGS": ["stitch tag"]
    }},
    {"EXECUTE_CHOOSE_STRING_PATH": "knot"},
    {"EXECUTE_STORY_CONTINUE": "Knot content\n"},
    {"TEST_CURRENT_TAGS": ["knot tag"]},
    {"EXECUTE_STORY_CONTINUE": ""},
    {"TEST_CURRENT_TAGS": ["end of knot tag"]},
]
INK_TEST_END
*/