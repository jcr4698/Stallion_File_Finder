all: search add_mult add_singl

search: search.c
	gcc -std=gnu99 -g -o search search.c

add_singl: add_single.c add_file.h
	gcc -std=gnu99 -g -o add_singl add_single.c add_file.h

add_mult: add_multiple.c add_file.h
	gcc -std=gnu99 -g -o add_mult add_multiple.c add_file.h
