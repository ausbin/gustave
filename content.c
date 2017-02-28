#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "repo.h"

/* Caller must free */
static char *escape_summary(char *summary) {
    size_t len = strlen(summary);
    // Worst-case, we're escaping every single character, so allocate
    // twice the space of the input string for our escaped string.
    char *escaped = malloc(2 * len + sizeof '\0');

    if (escaped == NULL) {
        return NULL;
    }

    int i, j;
    for (i = 0, j = 0; i < len; i++, j++) {
        char c = summary[i];
        if (c == '\\' || c == '"') {
            escaped[j] = '\\';
            j++;
        }
        escaped[j] = c;
    }
    escaped[j] = '\0';

    return escaped;
}

static int write_commit_content(FILE *fp, commit *c) {
    static const char date_fmt[] = "%FT%T-00:00";
    static const char date_fmt_example[] = "1970-01-01T00:00:00-00:00";
    static const char fmt[] = \
    "+++\n" \
    "type = \"commit\"\n" \
    "date = \"%s\"\n" \
    "draft = false\n" \
    "hash = \"%s\"\n" \
    "summary = \"%s\"\n" \
    "repo = \"%s\"\n" \
    "+++\n";

    char *commit_hash;
    char *escaped_summary;
    char formatted_date[sizeof date_fmt_example];
    struct tm *time = gmtime(&c->date);

    if (time == NULL) {
        return 1;
    }

    if (!strftime(formatted_date, sizeof date_fmt_example, date_fmt, time)) {
        return 1;
    }

    commit_hash = commit_hash_full(c);
    if (commit_hash == NULL) {
        return 1;
    }

    escaped_summary = escape_summary(c->summary);
    if (escaped_summary == NULL) {
        return 1;
    }

    if (fprintf(fp, fmt, formatted_date, commit_hash,
                escaped_summary, c->repo->name) < 0) {
        return 1;
    }

    free(escaped_summary);

    return 0;
}

int write_commit(commit *c) {
    static const char dir[] = "content/commit";
    static const char ext[] = ".md";
    char *abbrev_hash = commit_hash_abbrev(c);

    if (abbrev_hash == NULL) {
        return 1;
    }

    char path[sizeof dir-1 + sizeof '/' + 10 + sizeof ext-1 + 1];
    if (sprintf(path, "%s/%s.md", dir, abbrev_hash) < 0) {
        return 1;
    }

    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        return 1;
    }

    return write_commit_content(fp, c);
}
