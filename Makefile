all: search add_mult add_singl

search: search.c
	gcc -g -o search search.c

add_singl: add_single.c add_file.h
	gcc -g -o add_singl add_single.c add_file.h

add_mult: add_multiple.c add_file.h
	gcc -g -o add_mult add_multiple.c add_file.h

clean:
	-rm search add_singl add_mult
