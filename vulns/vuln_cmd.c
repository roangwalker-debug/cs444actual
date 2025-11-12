#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc > 1) {
        // Unsafe: passes untrusted input to the shell
        system(argv[1]);        // expect: "Command injection risk"
    }
    return 0;
}

