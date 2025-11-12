#include <stdio.h>

int main(int argc, char **argv) {
    if (argc > 1) {
        // Unsafe: attacker-controlled format string
        printf(argv[1]);        // expect: "Uncontrolled format string"
        printf("\n");
    }
    return 0;
}

