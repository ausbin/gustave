#include <stdio.h>
#include "cgitrc.h"
#include "commits.h"

int add_commit(commit *c, void *list) {
    return commit_list_add((commit_list *) list, c);
}

int main(int argc, char **argv) {
    FILE *fp;
    repo *head;
    commit_list *list;

    fp = fopen("cgitrc", "r");

    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    if (cgitrc_repos(fp, &head)) {
        perror("couldn't parse");
        return 1;
    }

    list = new_commit_list();

    if (list == NULL) {
        perror("new_commit_list");
    }

    while (head != NULL) {
        printf("name: %s, path: %s, hidden: %d\n",
               head->name, head->path, head->hidden);

        if (!head->hidden) {
            if (repo_commits(head, 32, add_commit, list)) {
                fprintf(stderr, "%s\n", repo_error());
            }
        }

        head = head->next;
    }

    /* Sort commits by date */
    commit_list_sort(list, 1);

    /* Print eight latest commits */
    for (int i = 0; i < 8 && i < list->size; i++) {
        commit *c = list->list[i];
        printf("%s: %s\n", c->repo->name, commit_hash_abbrev(c));
    }

    free_commit_list(list);
}
