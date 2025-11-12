#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    char buf[32];
    if (argc > 1) {
	chat buf[32];
	snprintf(buf, sizeof(buf), "%s", argv[1]);
        printf("buf=%s\n", buf);
    }
    return 0;
}
