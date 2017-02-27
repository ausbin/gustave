#include <stdlib.h>
#include "commits.h"

commit_list *new_commit_list() {
    static const int initial_capacity = 32;
    commit_list *list = malloc(sizeof (commit_list));

    if (list == NULL) {
        return NULL;
    }

    list->size = 0;
    list->capacity = initial_capacity;
    list->list = malloc(list->capacity * sizeof (commit *));

    if (list->list == NULL) {
        return NULL;
    }

    return list;
}

void free_commit_list(commit_list *list) {
    for (int i = 0; i < list->size; i++) {
        free_commit(list->list[i]);
    }
    free(list->list);
    free(list);
}

int commit_list_add(commit_list *list, commit *c) {
    static const int growth_factor = 2;

    if (list->size == list->capacity) {
        /* Warning: assumes list->capacity != 0 */
        list->capacity *= growth_factor;
        list->list = realloc(list->list, list->capacity * sizeof (commit *));

        if (list->list == NULL) {
            return 1;
        }
    }

    list->list[list->size++] = c;

    return 0;
}

static int compare_commits_age_asc (const void *a, const void *b) {
    return -commit_compare(*(commit **) a, *(commit **) b);
}

static int compare_commits_age_dsc (const void *a, const void *b) {
    return commit_compare(*(commit **) a, *(commit **) b);
}

void commit_list_sort(commit_list *list, int age_asc) {
    int (*cmp_function)(const void *a, const void *b);

    if (age_asc) {
        cmp_function = compare_commits_age_asc;
    } else {
        cmp_function = compare_commits_age_dsc;
    }

    qsort(list->list, list->size, sizeof (commit *), cmp_function);
}
