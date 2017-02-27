#include <stdio.h>
#include <string.h>
#include "cgitrc.h"
#include "commits.h"
#include "content.h"

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

    /* Sort commits by age ascending */
    commit_list_sort(list, 1);

    /* Print eight latest commits */
    for (int i = 0, j = 0; i < list->size && j < 8; i++) {
        commit *c = list->list[i];

        if (!strcmp(c->author_name, "Austin Adams")) {
            if (write_commit(c)) {
                perror("write_commit");
                return 1;
            }
            j++;
        }
    }

    free_commit_list(list);
}
