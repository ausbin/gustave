#include <time.h>
#include <stdio.h>
#include "repo.h"

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

    if (fprintf(fp, fmt, formatted_date, commit_hash,
                c->summary, c->repo->name) < 0) {
        return 1;
    }

    return 0;
}

int write_commit(commit *c) {
    static const char dir[] = "content/commits";
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
