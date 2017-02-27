#include <stdio.h>
#include <string.h>
#include "cgitrc.h"
#include "commits.h"
#include "content.h"

int add_commit(commit *c, void *list) {
    return commit_list_add((commit_list *) list, c);
}

int main(int argc, char **argv) {
    static const char default_cgitrc[] = "/etc/cgitrc";
    static const char desired_author[] = "Austin Adams";
    static const int max_commits = 8;
    const char *cgitrc;
    FILE *fp;
    repo *head;
    commit_list *list;

    /* Initialize libgit2 */
    repo_init();

    /* Use the cgitrc on the command line if supplied */
    if (argc > 1) {
        cgitrc = argv[1];
    } else {
        cgitrc = default_cgitrc;
    }

    fp = fopen(cgitrc, "r");

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
        if (!head->hidden) {
            if (repo_commits(head, 32, add_commit, list)) {
                fprintf(stderr, "%s\n", repo_error());
                return 1;
            }
        }

        head = head->next;
    }

    /* Sort commits by age ascending */
    commit_list_sort(list, 1);

    /* Write N latest commits to content/ */
    for (int i = 0, j = 0; i < list->size && j < max_commits; i++) {
        commit *c = list->list[i];

        if (!strcmp(c->author_name, desired_author)) {
            if (write_commit(c)) {
                perror("write_commit");
                return 1;
            }
            j++;
        }
    }

    free_commit_list(list);
}
