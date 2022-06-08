#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

FILE *gallery;
int p1[2];
pid_t pid;
int file_size;

int get_file_size(FILE* file_ptr, char* file_name);
void format_print(char* buff, int size);

int main(int argc, char* argv[]) {

	char* buff;
	
	/* set up pipe and child process */
	if(pipe(p1) == -1)
		exit(-1);
	if((pid = fork()) == -1)
		exit(-1);

	/* search for key words*/
	if(pid == 0) { // child
		dup2(p1[PIPE_WRITE], STDOUT_FILENO);
		execlp("grep", "grep", argv[2], argv[1], (char*) NULL);
	}
	else { // parent

		// calculate size of file
		file_size = get_file_size(gallery, argv[1]);

		// read and store contents of file into buff
		buff = malloc((file_size + 1) * sizeof(char));
		read(p1[PIPE_READ], buff, file_size);

		// print output
		format_print(buff, file_size);

		// free memory
		free(buff);
		WIFEXITED(pid);
	}
}

/* Open the file FILE_NAME and store into file_ptr,
 * then calculate its size. */
int get_file_size(FILE* file_ptr, char* file_name) {
	int size;
	file_ptr = fopen(file_name, "r");
	fseek(file_ptr, 0L, SEEK_END);
	size = ftell(file_ptr);
	fclose(file_ptr);
	return size;
}

/* Format the input BUFF of SIZE elements into
 * a formatted string, which is the name of the
 * new file. */
void format_print(char* buff, int size) {
	char file_name[100];
	int file_length = 0;
	for(int c = 0; c < size; c++) {
		file_name[file_length++] = buff[c];
		if(buff[c] == '<') {
			file_name[file_length - 1] = '\000';
			printf("%s\n", file_name);
			file_length = 0;
		}
		else if(buff[c] == '\n' || file_length == 100)
			file_length = 0;
	}
}
