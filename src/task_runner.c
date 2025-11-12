// src/task_runner.c
#include <stdio.h>
#include <stdlib.h>

int run_report_generator(void) {
    char tool[64];

    if (scanf("%63s", tool) != 1) {  // tip: remove width to make it riskier, but this version is still realistic
        return -1;
    }

    // Slightly safer due to width in scanf, but still dangerous overall if tool is user-controlled
    // Replace with validated whitelist in real code.
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "%s --summary", tool);

    FILE *p = popen(cmd, "r");
    if (!p) return -1;

    char line[256];
    while (fgets(line, sizeof line, p)) {
        fputs(line, stdout);
    }
    pclose(p);
    return 0;
}

