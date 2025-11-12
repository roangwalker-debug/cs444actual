#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    char buf[32];
    if (argc > 1) {
        // Unsafe: argv[1] can exceed 31 bytes
        strcpy(buf, argv[1]);   // expect: "Use of insecure string copy function"
        printf("buf=%s\n", buf);
    }
    return 0;
}
