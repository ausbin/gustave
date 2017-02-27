#include <time.h>

#ifndef REPO_H
#define REPO_H

typedef struct repo {
    char name[256];
    char path[256];
    int hidden;
    struct repo *next;
} repo;

typedef struct commit {
    repo *repo;
    unsigned char hash[20];
    char *author_name;
    char *author_email;
    char *summary;
    time_t date;
} commit;

char *repo_error();
int repo_commits(repo *r, int num, int (*for_each)(commit *, void *), void *userdata);
void free_commit(commit *c);
int commit_compare(commit *a, commit *b);
char *commit_hash_abbrev(commit *c);
char *commit_hash_full(commit *c);

#endif
