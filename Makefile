all: search add_mult add_singl adder finder

search: search.c
	gcc -g -o search search.c

add_singl: add_single.c add_file.h
	gcc -g -o add_singl add_single.c add_file.h

add_mult: add_multiple.c add_file.h
	gcc -g -o add_mult add_multiple.c add_file.h

adder: gui/file_adder.c gui/add_file_gui.h
	gcc -g -o adder gui/file_adder.c gui/add_file_gui.h `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`

finder: gui/file_finder.c gui/find_file_gui.h
	gcc -g -o finder gui/file_finder.c gui/find_file_gui.h `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`

clean:
	-rm search add_singl add_mult adder finder
