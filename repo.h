#include <time.h>
#include "cgitrc.h"

typedef struct {
    repo   *repo;
    char   hash[40];
    char   summary[51];
    time_t  date;
} commit;
