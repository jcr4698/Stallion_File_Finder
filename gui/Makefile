all: adder finder

adder: file_adder.c add_file_gui.h
	gcc -g -o adder file_adder.c add_file_gui.h `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`

finder: file_finder.c find_file_gui.h
	gcc -g -o finder file_finder.c find_file_gui.h `pkg-config --cflags gtk+-3.0 --libs gtk+-3.0`

clean:
	-rm adder finder