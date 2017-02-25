#include <stdio.h>

typedef struct repo {
    char name[256];
    char path[256];
    int hidden;
    struct repo *next;
} repo;

int cgitrc_repos(FILE *fp, repo **result);
