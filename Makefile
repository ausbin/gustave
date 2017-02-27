gustave: gustave.c cgitrc.c repo.c commits.c repo.h commits.h
	gcc -g gustave.c cgitrc.c repo.c commits.c -lgit2 -o gustave
