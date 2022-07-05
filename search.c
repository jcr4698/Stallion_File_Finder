#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define TRUE 1
#define FALSE 0
#define GALLERY_NAME "Gallery_Content.txt"
#define FORMAT_COLUMN 4
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define DEFAULT "\033[0m"

FILE *gallery;
int p1[2];
int p2[2];
int p3[2];
pid_t pid;
pid_t cpid;
pid_t ccpid;
int file_size;
_Bool* was_found;
char* buff;
int line_count;

_Bool search_file_for(char* cmd[], int idx);
void search_file_coincidences(char* cmd[], int char_count, int word_count);
int get_file_size(FILE* file_ptr, char* file_name);
void format_print(char* buff, int size, int line_amt);
void awk_format_print(char* awk_search, char* cmd[], int char_count, int word_count);
void change_text_color(char* color);
char opp_case(char c);

int main(int argc, char* argv[]) {
	int char_count = 0;
	was_found = malloc((argc - 1) * sizeof(_Bool));

	for(int i = 1; i < argc; i++) {
		was_found[i] = search_file_for(argv, i);
		if(was_found[i])
			char_count += strlen(argv[i]) + 6;	// count chars for coincidences
	}
	if(argc > 2)
		search_file_coincidences(argv, char_count, argc);

	return 0;
}

_Bool search_file_for(char* cmd[], int idx) {
	
	/* set up pipe 1 and child 1 */
	if(pipe(p1) == -1)
		exit(-1);
	if((pid = fork()) == -1)
		exit(-1);

	/* search for key words*/
	else if(pid == 0) {
		dup2(p1[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
		execlp("grep", "grep", "-c", "-i", cmd[idx], GALLERY_NAME, (char*) NULL);
		exit(0);
	}
	else {
		wait(NULL);

		change_text_color(CYAN);
		if(idx != 1)
			printf("\n");
		printf("\"%s\":\n", cmd[idx]);	// print word we're trying to find
		change_text_color(DEFAULT);

		file_size = get_file_size(gallery, GALLERY_NAME);
		buff = malloc((file_size + 1) * sizeof(char));	// allocate buffer

		read(p1[PIPE_READ], buff, 4);
		line_count = atoi(buff);		// lines to be printed

		if(line_count > 0) {
			/* set up pipe 2 and child 2 of parent 1*/
			if(pipe(p2) == -1)
				exit(-1);
			if((cpid = fork()) == -1)
				exit(-1);
			else if(cpid == 0) {
				dup2(p2[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
				execlp("grep", "grep", "-i", cmd[idx], GALLERY_NAME, (char*) NULL);
				exit(0);
			}

			/* parent process 2 */
			else {
				wait(NULL);
				read(p2[PIPE_READ], buff, file_size);	// read and store
				format_print(buff, file_size, line_count);	// print file names
			}
		}

		change_text_color(GREEN);
		if(line_count == 0)
			change_text_color(RED);
		printf("--> %d file(s) found w/ key \"%s\"\n", line_count, cmd[idx]);	// print file count found
		change_text_color(DEFAULT);
	}

	/* clear saved variables */
	free(buff);

	if(line_count == 0)
		return FALSE;
	return TRUE;
}

void search_file_coincidences(char* cmd[], int char_count, int word_count) {

	/* parse the parameters of command */
	int cmd_length = char_count + 3;
	char* awk_search = malloc(cmd_length * sizeof(char));
	awk_format_print(awk_search, cmd, char_count, word_count);

	/* set up pipe 3 and child 3 for 'awk' command */
	if(pipe(p1) == -1)
		exit(-1);
	if((pid = fork()) == -1)
		exit(-1);

	/* run 'awk' command */
	else if(pid == 0) {
		dup2(p1[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
		close(p1[0]);
        close(p1[1]);
		execlp("awk", "awk", "-b", awk_search, GALLERY_NAME, (char*) NULL);
	}
	else {

		file_size = get_file_size(gallery, GALLERY_NAME);
		buff = malloc((file_size + 1) * sizeof(char));	// allocate buffer

		dup2(p1[PIPE_READ], STDIN_FILENO);	// pipe child and parent process
		close(p1[0]);
        close(p1[1]);

		if(pipe(p2) == -1)
			exit(-1);
		if((cpid = fork()) == -1)
			exit(-1);
		else if(cpid == 0) {
			dup2(p2[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
			execlp("wc", "wc", "-l", NULL);
		}
		else {
			wait(NULL);

			read(p2[PIPE_READ], buff, 4);	// read and store
			line_count = atoi(buff);
			
			if(line_count > 0) {
				/* set up pipe 2 and child 2 of parent 1*/
				if(pipe(p3) == -1)
					exit(-1);
				if((ccpid = fork()) == -1)
					exit(-1);
				else if(ccpid == 0) {
					dup2(p3[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
					execlp("awk", "awk", "-b", awk_search, GALLERY_NAME, (char*) NULL);
					exit(0);
				}

				/* parent process 2 */
				else {
					wait(NULL);
					read(p3[PIPE_READ], buff, file_size);	// read and store
					format_print(buff, file_size, line_count);	// print file names
				}
			}
			change_text_color(GREEN);
			printf("--> %d file(s) found w/ key \"%s\"\n", line_count, awk_search);	// print file count found
			change_text_color(DEFAULT);
		}
	}

	close(p1[0]);
    close(p1[1]);

	waitpid(pid, NULL, 0);
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

/* Format the input BUFF of SIZE characters into
 * a formatted string of LINE_AMT lines, which is
 * the name of the new file. */
void format_print(char* buff, int size, int line_amt) {
	int curr_line = 0;
	char file_name[100];
	int file_length = 0;
	for(int c = 0; (c < size) && (curr_line != line_amt); c++) {
		file_name[file_length++] = buff[c];
		if(buff[c] == '<') {
			file_name[file_length - 1] = '\000';
			printf("%-35s ", file_name);
			if((curr_line + 1) % FORMAT_COLUMN == 0)
				printf("\n");
			curr_line++;
			file_length = 0;
		}
		else if(buff[c] == '\n' || file_length == 100)
			file_length = 0;
	}
	if((curr_line % FORMAT_COLUMN) != 0)
		printf("\n");
}

/* parse and print the parameters of command */
void awk_format_print(char* awk_search, char* cmd[], int char_count, int word_count) {
	printf("\n");
	change_text_color(CYAN);
	awk_search[0] = '\000';
	for(int i = 1; i < word_count; i++) {
		if(was_found[i]) {
			// store string into key
			strcat(awk_search, "/");
			strcat(awk_search, cmd[i]);
			strcat(awk_search, "/ && ");

			// print word being searched
			printf("\"%s\"", cmd[i]);
			if(i < word_count - 1)
				printf(", ");
		}
	}
	awk_search[char_count - 4] = '\000';
	printf(":\n");
	change_text_color(DEFAULT);
}

/* change text color to COLOR, don't forget
 * to change it back to DEFAULT after */
void change_text_color(char* color) {
	printf("%s", color);
}

/* Changes character C from upper-case to lower-case,
 * or lower-case to upper case and returns it. */
char opp_case(char c) {
    if(c > 96 && c < 123)
        return c - 32;
	else if(c > 64 && c < 91)
        return c + 32;
    return c;
}
