#include <git2.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "repo.h"

static const git_error *current_error = NULL;

void repo_init() {
    git_libgit2_init();
}

char *repo_error() {
    if (current_error == NULL) {
        return NULL;
    } else {
        char *result = current_error->message;
        current_error = NULL;
        return result;
    }
}

static commit *new_commit(repo *r, git_commit *gitcommit) {
    commit *result = malloc(sizeof (commit));
    result->repo = r;
    memcpy(result->hash, git_commit_id(gitcommit), sizeof result->hash);

    const char *summary = git_commit_summary(gitcommit);
    if (summary == NULL) {
        current_error = giterr_last();
        return NULL;
    }
    strncpy(result->summary, summary, sizeof result->summary - 1);
    result->summary[sizeof result->summary - 1] = '\0';

    result->date = git_commit_time(gitcommit);
    return result;
}

void free_commit(commit *c) {
    free(c);
}

int commit_compare(commit *a, commit *b) {
    double diff_seconds = difftime(a->date, b->date);

    /* This tedious if prevents truncation/overflow issues */
    if (diff_seconds < 0) {
        return -1;
    } else if (diff_seconds > 0) {
        return 1;
    } else {
        return 0;
    }
}

/* Caller needs to free() */
char *commit_hash_abbrev(commit *c) {
    char *dest = malloc((10 + 1) * sizeof (char));

    if (dest == NULL) {
        return NULL;
    }

    if (sprintf(dest, "%02x%02x%02x%02x%02x",
                c->hash[0],
                c->hash[1],
                c->hash[2],
                c->hash[3],
                c->hash[4]) < 0) {
        return NULL;
    }

    return dest;
}

struct commit_queue {
    git_commit *commit;
    struct commit_queue *next;
};

static struct commit_queue *new_commit_queue(git_commit *commit) {
    struct commit_queue *queue_entry = malloc(sizeof (struct commit_queue));
    queue_entry->commit = commit;
    queue_entry->next = NULL;
    return queue_entry;
}

static git_commit *dequeue(struct commit_queue **head,
                           struct commit_queue **tail) {
    struct commit_queue *oldhead = *head;
    git_commit *commit = oldhead->commit;
    *head = oldhead->next;

    if (*head == NULL) {
        *tail = NULL;
    }

    free(oldhead);

    return commit;
}

static void *enqueue(git_commit *commit,
                     struct commit_queue **head,
                     struct commit_queue **tail) {
    struct commit_queue *queue_entry = new_commit_queue(commit);

    if (*head == NULL) {
        *head = queue_entry;
    } else {
        (*tail)->next = queue_entry;
    }

    *tail = queue_entry;
}

static int repo_bfs(repo *r, git_commit *start, int n, int (*for_each)(commit *, void *), void *userdata) {
    int visited_count;
    git_commit **visited;
    struct commit_queue *queue_head, *queue_tail;

    visited_count = 0;
    visited = malloc(sizeof (git_commit *) * n);

    /* Enqueue start commit */
    queue_head = queue_tail = new_commit_queue(start);
    /* Add start to visited */
    visited[visited_count++] = start;

    if (for_each(new_commit(r, start), userdata)) {
        return 2;
    }

    while (queue_head != NULL && visited_count < n) {
        git_commit *commit = dequeue(&queue_head, &queue_tail);
        int num_parents = git_commit_parentcount(commit);
        git_commit *parent;

        for (int j = 0; j < num_parents; j++) {
            if (git_commit_parent(&parent, commit, j)) {
                goto error;
            }

            /* XXX Find a sublinear way to do this */
            int was_visited = 0;
            for (int i = 0; i < visited_count; i++) {
                if (visited[i] == parent) {
                    was_visited = 1;
                    break;
                }
            }

            if (!was_visited) {
                enqueue(parent, &queue_head, &queue_tail);
                visited[visited_count++] = parent;

                if (for_each(new_commit(r, parent), userdata)) {
                    return 2;
                }
            }
        }
    }

    return 0;

    error:
    current_error = giterr_last();
    return 1;
}

int repo_commits(repo *r, int num, int (*for_each)(commit *, void *), void *userdata) {
    git_repository *gitrepo;
    git_reference *headref;
    git_object *obj;
    git_commit *head;

    if (git_repository_open(&gitrepo, r->path))
        goto git_error;

    if (git_branch_lookup(&headref, gitrepo, "master", GIT_BRANCH_LOCAL))
        goto git_error;

    if (git_reference_peel(&obj, headref, GIT_OBJ_COMMIT))
        goto git_error;

    if (git_commit_lookup(&head, gitrepo, git_object_id(obj)))
        goto git_error;

    /* repo_bfs() sets current_error itself on failure */
    if (repo_bfs(r, head, num, for_each, userdata))
        return 2;

    return 0;

    git_error:
    current_error = giterr_last();
    return 1;

}
