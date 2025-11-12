// src/file_ops.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int write_temp_note(const char *note) {
    char tmpname[L_tmpnam];

    if (tmpnam(tmpname) == NULL) return -1;//edit

    FILE *f = fopen(tmpname, "w");
    if (!f) return -1;

    fputs(note, f);
    fclose(f);
    return 0;
}

int archive_outputs(const char *outFile, const char *folder) {
    char cmd[256];

    sprintf(cmd, "tar -czf %s %s", outFile, folder);

    return system(cmd);
}

