#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
// #define GALLERY_NAME "Gallery_Content.txt"
#define CACHE "./cache"
#define GALLERY_NAME "/mnt/c/Users/jan/Desktop/Files/Work_Material/Employment/Visualizations_Lab/Display_Cluster/Stallion_File_Finder/Mock_Gallery_Content.txt"
#define FORMAT_COLUMN 4
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define DEFAULT "\033[0m"

FILE *gallery;
FILE* cache_ptr;
int p1[2];
int p2[2];
int p3[2];
pid_t pid;
pid_t cpid;
pid_t ccpid;
int file_size;
_Bool* was_found;
int amt_found;
char* buff;
int line_count;

_Bool search_file_for(char* key_word);
void search_file_coincidences(char* awk_search);
int get_file_size(FILE* file_ptr, char* file_name);
void format_print(char* buff, int size, int line_amt);
void awk_format_print(char* awk_lookup, char* awk_search);
void change_text_color(char* color);
char opp_case(char c);

int search(int word_count, char* key_words, int char_count) {

	/* Store variables */
	was_found = malloc((word_count) * sizeof(_Bool));
	amt_found = 0;
    char *str, *token;
    int tok = 0;

	/* Open cache file */
	cache_ptr = fopen(CACHE, "w+");
	if(cache_ptr == NULL)
		printf("Cache Error: cache file in find_file_gui.h: Line 52 did not open!");

	/* Storage for coincidences */
	char* awk_lookup = malloc((strlen(key_words) + (4 * word_count) + 1) * sizeof(char));
	strcpy(awk_lookup, "");
	char* awk_search = malloc(char_count * sizeof(char));
	strcpy(awk_search, "");
	int amt_printed = 0;

	/* Prepare for parse */
    str = malloc((strlen(key_words) + 1) * sizeof(char));
    strcpy(str, key_words);

	/* Parse and search for tokens*/
    while(token = strtok_r(str, " ", &str)) {
		tok++;
		was_found[tok] = search_file_for(token);
		if(was_found[tok]) {
			// increment key word found
			amt_found++;

			// store string into key
			strcat(awk_search, "/");
			strcat(awk_search, token);
			strcat(awk_search, "/ && ");

			// store word being searched
			strcat(awk_lookup, "\"");
			strcat(awk_lookup, token);
			strcat(awk_lookup, "\", ");
		}
    }

	if((tok > 1) && (amt_found > 1)) {
		/* Format and print coincidence */
		awk_format_print(awk_lookup, awk_search);

		/* Show coincidences found */
		search_file_coincidences(awk_search);
	}

	/* close cache file */
	fclose(cache_ptr);

	/* Deallocate memory */
	free(token);
	free(awk_lookup);
	free(awk_search);

	return 0;
}

/* Looks up index IDX from array CMD, which is a string. The grep command is
 * used to look up this string within the file named "Gallery_Content.txt". */
_Bool search_file_for(char* key_word) {
	
	/* set up pipe 1 and child 1 */
	if(pipe(p1) == -1)
		exit(-1);
	if((pid = fork()) == -1)
		exit(-1);

	/* search for key words*/
	else if(pid == 0) {
		dup2(p1[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
		execlp("grep", "grep", "-c", "-i", key_word, GALLERY_NAME, (char*) NULL);
		exit(0);
	}
	else {
		wait(NULL);

		change_text_color(CYAN);
		printf("\n\"%s\":\n", key_word);	// print word we're trying to find
		fprintf(cache_ptr, "\n\"%s\":\n", key_word);
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
				execlp("grep", "grep", "-i", key_word, GALLERY_NAME, (char*) NULL);
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
		printf("--> %d file(s) found w/ key \"%s\"\n", line_count, key_word);	// print file count found
		fprintf(cache_ptr, "--> %d file(s) found w/ key \"%s\"\n", line_count, key_word);
		change_text_color(DEFAULT);
	}

	/* clear saved variables */
	free(buff);

	if(line_count == 0)
		return 0;
	return 1;
}

/* Parses and stores all WORD_COUNT strings from CMD that were previously found.
 * The awk command is used to look up lines from "Gallery_Content.txt". that contain
 * all these keywords in common. */
void search_file_coincidences(char* awk_search) {

	/* set up pipe 3 and child 3 for 'awk' command */
	if(pipe(p1) == -1)
		exit(-1);
	if((pid = fork()) == -1)
		exit(-1);

	/* run 'awk' command */
	else if(pid == 0) {
		dup2(p1[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
		close(p1[PIPE_READ]);
        close(p1[PIPE_WRITE]);
		execlp("awk", "awk", "-b", awk_search, GALLERY_NAME, (char*) NULL);
	}
	else {

		file_size = get_file_size(gallery, GALLERY_NAME);
		buff = malloc((file_size + 1) * sizeof(char));	// allocate buffer

		dup2(p1[PIPE_READ], STDIN_FILENO);	// pipe child and parent process
		close(p1[PIPE_READ]);
        close(p1[PIPE_WRITE]);

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
			fprintf(cache_ptr, "--> %d file(s) found w/ key \"%s\"\n", line_count, awk_search);
			change_text_color(DEFAULT);
		}
	}

	close(p1[PIPE_READ]);
    close(p1[PIPE_WRITE]);

	waitpid(pid, NULL, 0);
}

/* Open the file FILE_NAME and read it's contents into new_buff */
char* read_file(FILE* file_ptr, char* file_name, int file_size) {
    char* new_buff;
    new_buff = malloc((file_size + 1) * sizeof(char));
    file_ptr = fopen(file_name, "rw");
    fseek(file_ptr, 0, SEEK_SET);
    fread(new_buff, file_size, 1, file_ptr); // read contents of file
    fclose(file_ptr);
    return new_buff;
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

/* Format and print the key words that were found */
void awk_format_print(char* awk_lookup, char* awk_search) {
	/* Format and print coincidence */
	awk_lookup[strlen(awk_lookup) - 2] = '\000';
	awk_search[strlen(awk_search) - 4] = '\000';
	printf("\n");
	change_text_color(CYAN);
	printf("%s", awk_lookup);
	printf(":\n");
	fprintf(cache_ptr, "\n%s:\n", awk_lookup); // save to cache
	change_text_color(DEFAULT);
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
			fprintf(cache_ptr, "%-50s", file_name);	// save to cache
			if((curr_line + 1) % FORMAT_COLUMN == 0) {
				printf("\n");
				fprintf(cache_ptr, "\n");	// save to cache
			}
			curr_line++;
			file_length = 0;
		}
		else if(buff[c] == '\n' || file_length == 100)
			file_length = 0;
	}
	if((curr_line % FORMAT_COLUMN) != 0) {
		printf("\n");
		fprintf(cache_ptr, "\n");	// save to cache
	}
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
