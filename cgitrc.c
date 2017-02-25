#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "repo.h"

/* Extract a value from a line of key=value */
void cgitrc_extract(char *dest, char *src, size_t skip, size_t max) {
    int i;
    char *start = src + skip;

    for (i = 0; i < max - 1 && start[i] != '\0' && start[i] != '\n'; i++);

    if (i >= max) {
        i = max - 1;
    }

    memcpy(dest, start, i);
    dest[i] = '\0';
}

/* Read fp line by line, creating a linked list of repositories */
int cgitrc_repos(FILE *fp, repo **result) {
    static const char repo_url[] = "repo.url=";
    static const char repo_path[] = "repo.path=";
    static const char repo_hide[] = "repo.hide=1\n";
    char line[512];
    repo *head;

    head = NULL;

    while (fgets(line, sizeof line, fp) != NULL) {
        /* New repository found */
        if (!strncmp(repo_url, line, sizeof repo_url - 1)) {
            repo *r = malloc(sizeof (repo));

            /* Memory allocation issue */
            if (r == NULL) {
                return 1;
            }

            /* Setup new repo */
            cgitrc_extract(r->name, line, sizeof repo_url - 1, sizeof r->name);
            r->path[0] = '\0';
            r->hidden = 0;

            r->next = head;
            head = r;
        }

        if (head != NULL && !strncmp(repo_path, line, sizeof repo_path - 1)) {
            cgitrc_extract(head->path, line, sizeof repo_path - 1, sizeof head->path);
        }

        if (head != NULL && !strncmp(repo_hide, line, sizeof repo_hide)) {
            head->hidden = 1;
        }
    }

    /* Check for fgets() read error (caller can handle errno) */
    if (ferror(fp)) {
        return 1;
    }

    *result = head;

    return 0;
}
