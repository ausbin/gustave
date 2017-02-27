#include "repo.h"

typedef struct commit_list {
    commit **list;
    int size;
    int capacity;
} commit_list;

commit_list *new_commit_list();
void free_commit_list(commit_list *list);
int commit_list_add(commit_list *list, commit *c);
void commit_list_sort(commit_list *list, int age_asc);
