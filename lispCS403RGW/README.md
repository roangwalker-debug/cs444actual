CS403 - Lisp Interpreter sprints 1 - 8
it includes everything detailed in sprints 1 through 8:
    reader/printer, datatypes and structures, operators, functions, eval etc.

BUILD INSTRUCTIONS
Requirements:
    C compiler that supports C11
    Mac and Linux
        gcc -std=c11 -Wall -Wextra -02 -o sexp sprintsFinished.c
    Windows
        gcc -std=c11 -Wall -Wextra -02 -o sexp.exe sprintsFinished.c

RUNNING
The program starts and provides an option to the user to run the built-in test code or to start a interactive response lisp interpreter
    Type 1 and Enter for the test cases
    Type 2 and Enter for the REPL

FILES
Breakdown of Files included
    The sprints are all combined into one program called sprintsFinished.c
        Inside, for the most part, all code is designated by which sprint it came from but some are not labeled.

        Included is also a test block to run. It tests everything from all the sprints individually.
    README.md - which is this file
        Info on the program: build, overview, and special notes
    testOut.txt
        caputured output from a run of the program on my machine
        I used this code and the mac compile to run it
        "printf "1\n" | ./sexp > testOut.txt"

SPECIAL NOTES
isTrue function is for false and nil so false reacts same as nil which is weird but had to do it

I used lisp-1 from sprint 7 so functions and variables share the same environment and can't have the same name

I did not include a full garbage collector but I believe I added enough to not cause memory leakage

Improvements
    I could include a makefile
    I could add better comments
    I could test more big picture things but I still think my code is extensive enough
    