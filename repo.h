#include <time.h>

typedef struct repo {
    char name[256];
    char path[256];
    int hidden;
    struct repo *next;
} repo;

typedef struct commit {
    repo   *repo;
    char   hash[40];
    char   summary[51];
    time_t  date;
} commit;
