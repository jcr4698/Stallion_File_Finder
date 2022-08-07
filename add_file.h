#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define PIPE_READ 0
#define PIPE_WRITE 1
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define DEFAULT "\033[0m"
#define GALLERY_NAME "/home/programmed_bean/stallion/Gallery_Content.txt"
#define CONTENTS_FOLDER "/home/programmed_bean/stallion/Contents"
#define COMMON_FOLDER "/home/programmed_bean/stallion"

char* read_file(FILE* file_ptr, char* file_name, int file_size);
int get_file_size(FILE* file_ptr, char* file_name);
_Bool file_exists(char* file_name, char* dir);
char* strtrim(char* str);
void add_to_gallery(FILE* file_ptr, char* new_name, char* old_name, char* desc);
char* print_format(char* str1, char* str2, char* str3, int size);
void make_symbolic_link(pid_t pid, char* contents, char* new_name, char* old_name);
int sym_link_valid(FILE* file_ptr, pid_t pid, int* p1, char* old_name, int file_size);
void change_text_color(char* color);

char* read_file(FILE* file_ptr, char* file_name, int file_size) {
    char* new_buff = malloc((file_size + 1) * sizeof(char));
    file_ptr = fopen(file_name, "rw");
    if(file_ptr != NULL) {
        char ch = ' ';
        for(int c = 0; c < file_size && ((ch = getc(file_ptr)) != EOF); c++)
            new_buff[c] = ch;
        fclose(file_ptr);
    }
    return new_buff;
}

/* Open the file FILE_NAME and store into FILE_PTR,
 * then calculate and returns its size. */
int get_file_size(FILE* file_ptr, char* file_name) {
	int size = 0;
	file_ptr = fopen(file_name, "r");
	fseek(file_ptr, 0L, SEEK_END);
	size = ftell(file_ptr);
	fclose(file_ptr);
	return size;
}

_Bool file_exists(char* file_name, char* dir) {
    char* abs_file_name = malloc((strlen(dir) + strlen(file_name) + 2) * sizeof(char));
    strcpy(abs_file_name, "");
    strcat(abs_file_name, dir);
    strcat(abs_file_name, "/");
    strcat(abs_file_name, file_name);
    if(fopen(abs_file_name, "r") != NULL)
        return TRUE;
	return FALSE;
}

char* strtrim(char* str) {
    int curr_r = strlen(str);
    if((str[0] == ' ') || (str[0] == '\n') || (str[curr_r - 1] == ' ') || (str[curr_r - 1] == '\n')) {
        int curr_l = 0;
        for(int idx = curr_r - 1; idx > -1; idx--) {
            if(str[idx] == ' ' || str[idx] == '\n')
                curr_r = idx;
            else
                break;
        }
        for(int idx = 0; idx < curr_r; idx++) {
            curr_l = idx;
            if(str[idx] != ' ' && str[idx] != '\n')
                break;
        }
        int newsize = curr_r - curr_l;
        char* newstr;
        if(newsize == 0) {
            newstr = malloc(1 * sizeof(char));
            strcpy(newstr, "");
            return newstr;
        }
        newstr = malloc((newsize + 1) * sizeof(char));
        int curr_idx = 0;
        for(int idx = curr_l; idx < curr_r; idx++)
            newstr[curr_idx++] = str[idx];
        newstr[newsize] = '\000';
        return newstr;
    }
    return str;
}

void add_to_gallery(FILE* file_ptr, char* new_name, char* old_name, char* desc) {
    /* format the output string */
    int str_size = strlen(new_name) + 4 + strlen(desc) + 3;
    if(old_name != NULL)
        str_size += strlen(old_name) + 2;
    char* str_add = malloc(str_size * sizeof(char));
    str_add = print_format(new_name, old_name, desc, str_size);

    printf("%s\n\n", strtrim(str_add));

    /* add string to 'Gallery_Content.txt' */
    file_ptr = fopen(GALLERY_NAME, "a");
    if(file_ptr == NULL)
        printf("Error opening file.\n");
    fputs(str_add, file_ptr);
}

char* print_format(char* str1, char* str2, char* str3, int size) {
    char* str = malloc(size * sizeof(char));
    strcat(str, "\n");
    strcat(str, str1);
    strcat(str, " <- ");
    if((str2 != NULL) && (strlen(str2) > 0)) {
        strcat(str, str2);
        strcat(str, ", ");
    }
    strcat(str, str3);
    strcat(str, "\n");
    return str;
}

void make_symbolic_link(pid_t pid, char* contents, char* new_name, char* old_name) {
    char* new_abs_name = malloc((strlen(CONTENTS_FOLDER) + strlen(new_name) + 2) * sizeof(char));
    char* old_abs_name = malloc((strlen(COMMON_FOLDER) + strlen(old_name) + 2) * sizeof(char));
    strcat(new_abs_name, CONTENTS_FOLDER);
    strcat(new_abs_name, "/");
    strcat(new_abs_name, new_name);
    strcat(old_abs_name, COMMON_FOLDER);
    strcat(old_abs_name, "/");
    strcat(old_abs_name, old_name);
	if((pid = fork()) == -1)
		exit(-1);
	else if(pid == 0)
		execlp("ln", "ln", "-s", old_abs_name, new_abs_name, (char*) NULL);
}

int sym_link_valid(FILE* file_ptr, pid_t pid, int* p, char* old_name, int file_size) {
    
    if(pipe(p) == -1)
		exit(-1);
	if((pid = fork()) == -1)
		exit(-1);
    else if(pid == 0) {
		dup2(p[PIPE_WRITE], STDOUT_FILENO);	// pipe child and parent process
		execlp("grep", "grep", "-c", "-i", old_name, GALLERY_NAME, (char*) NULL);
        exit(0);
	}
    else {
        wait(NULL);
        char num[5];
        read(p[PIPE_READ], num, 4);
        // printf("found %s: %s\n", old_name, num);
        if((num != NULL) && ((strlen(num)) > 0))
            return atoi(num);
        return 0;
    }
}

/* change text color to COLOR, don't forget
 * to change it back to DEFAULT after */
void change_text_color(char* color) {
	printf("%s", color);
}
