Inspector Gustave
=================

Collects the most recent commits from the repositories in a `cgitrc` and writes
them as content for Hugo.

For details, please see [my blog post about showing recent git commits in Hugo
pages][1].

Because this is a quick script written for me (and in C because I wanted it to
be as fast as possible), it's not pretty, but improvements are always welcome.

Process
-------

 1. Read a `cgitrc` to determine M repos to scan
 2. Fetch the first N commits from each repo
 3. Sort M\*N commits, finding N newest
 4. Write N newest to `content/commits/<hash>.md`, with date set to commit
    dates and .Params containing the metadata.

[1]: https://austinjadams.com/blog/including-recent-commits-hugo/
