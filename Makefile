gustave: gustave.c cgitrc.c repo.c commits.c repo.h commits.h content.c content.h
	gcc -std=gnu99 -g gustave.c cgitrc.c repo.c commits.c content.c -lgit2 -o gustave
