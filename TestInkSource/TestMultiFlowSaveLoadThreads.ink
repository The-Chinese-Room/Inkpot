Default line 1
Default line 2

== red ==
Hello I'm red
<- thread1("red")
<- thread2("red")
-> DONE

== blue ==
Hello I'm blue
<- thread1("blue")
<- thread2("blue")
-> DONE

== thread1(name) ==
+ Thread 1 {name} choice
    -> thread1Choice(name)

== thread2(name) ==
+ Thread 2 {name} choice
    -> thread2Choice(name)

== thread1Choice(name) ==
After thread 1 choice ({name})
-> END

== thread2Choice(name) ==
After thread 2 choice ({name})
-> END

/*
INK_TEST_STORY_START
[
	{"TEST_ERROR_EQUAL": 0},
	{"TEST_WARNING_EQUAL": 0},

	{"EXECUTE_STORY_CONTINUE": "Default line 1\n"},

    {"EXECUTE_SWITCH_FLOW": "Blue Flow"},
    {"EXECUTE_CHOOSE_STRING_PATH":"blue"},
	{"EXECUTE_STORY_CONTINUE": "Hello I'm blue\n"},

    {"EXECUTE_SWITCH_FLOW": "Red Flow"},
    {"EXECUTE_CHOOSE_STRING_PATH":"red"},
	{"EXECUTE_STORY_CONTINUE": "Hello I'm red\n"},

    {"EXECUTE_SWITCH_FLOW": "Blue Flow"},
    {"TEST_CURRENT_STORY_TEXT": "Hello I'm blue\n"},
    {"TEST_CHOICE_TEXT": [0, "Thread 1 blue choice"]},

    {"EXECUTE_SWITCH_FLOW": "Red Flow"},
    {"TEST_CURRENT_STORY_TEXT": "Hello I'm red\n"},
    {"TEST_CHOICE_TEXT": [0, "Thread 1 red choice"]},

    {"EXECUTE_SAVE_JSON_STATE": "JSON"},

    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Thread 1 red choice\nAfter thread 1 choice (red)\n"},
    {"EXECUTE_RESET": 0},

    {"EXECUTE_LOAD_JSON_STATE": "JSON"},
    {"EXECUTE_STORY_CHOICE": 1},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Thread 2 red choice\nAfter thread 2 choice (red)\n"},

    {"EXECUTE_LOAD_JSON_STATE": "JSON"},
    {"EXECUTE_SWITCH_FLOW": "Blue Flow"},
    {"EXECUTE_STORY_CHOICE": 0},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Thread 1 blue choice\nAfter thread 1 choice (blue)\n"},

    {"EXECUTE_LOAD_JSON_STATE": "JSON"},
    {"EXECUTE_SWITCH_FLOW": "Blue Flow"},
    {"EXECUTE_STORY_CHOICE": 1},
    {"EXECUTE_CONTINUE_MAXIMALLY": "Thread 2 blue choice\nAfter thread 2 choice (blue)\n"},

    {"EXECUTE_REMOVE_FLOW": "Blue Flow"},
	{"EXECUTE_STORY_CONTINUE": "Default line 2\n"}
]
INK_TEST_END
*/