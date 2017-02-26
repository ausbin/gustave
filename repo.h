#include <time.h>

typedef struct repo {
    char name[256];
    char path[256];
    int hidden;
    struct repo *next;
} repo;

typedef struct commit {
    repo   *repo;
    unsigned char   hash[20];
    char   summary[51];
    time_t  date;
} commit;

char *repo_error();
int repo_commits(repo *r, int num, void (*for_each)(commit *));
