#include <stdio.h>
#include "cgitrc.h"

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
        head = head->next;
    }
}
