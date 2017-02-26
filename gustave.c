#include <stdio.h>
#include "cgitrc.h"

void print_commit(commit *commit) {
    printf("hash: %02x%02x%02x%02x\n",
           commit->hash[0],
           commit->hash[1],
           commit->hash[2],
           commit->hash[3]);
}

int main(int argc, char **argv) {
    FILE *fp;
    repo *head;

    fp = fopen("cgitrc", "r");

    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    if (cgitrc_repos(fp, &head)) {
        perror("couldn't parse");
        return 1;
    }

    while (head != NULL) {
        printf("name: %s, path: %s, hidden: %d\n",
               head->name, head->path, head->hidden);

        if (!head->hidden) {
            if (repo_commits(head, 32, print_commit)) {
                fprintf(stderr, "%s\n", repo_error());
            }
        }

        head = head->next;
    }
}
