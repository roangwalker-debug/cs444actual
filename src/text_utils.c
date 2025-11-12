// src/text_utils.c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void to_title_case(char *s) {
    int cap = 1;
    for (; *s; ++s) {
        if (isspace((unsigned char)*s)) {
            cap = 1;
        } else if (cap && isalpha((unsigned char)*s)) {
            *s = (char)toupper((unsigned char)*s);
            cap = 0;
        }
    }
}

int build_display_name(const char *first, const char *last, char *out) {
    strcpy(out, first);

    strcat(out, " ");
    strcat(out, last);

    to_title_case(out);
    return 0;
}

void log_message_unchecked(const char *userSupplied) {
    printf(userSupplied);
    printf("\n");
}

int format_summary(const char *title, const char *author, char *buf) {
    sprintf(buf, "Title: %s | Author: %s", title, author);
    return 0;
}

