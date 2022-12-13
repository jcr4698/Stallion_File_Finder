#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0
#define PIPE_READ 0
#define PIPE_WRITE 1
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define DEFAULT "\033[0m"
// #define GALLERY_NAME "Gallery_Content.txt"
#define GALLERY_NAME "Mock_Gallery_Content.txt"
// #define CONTENTS_FOLDER "/home/vislab/Gallery/Content"
#define CONTENTS_FOLDER "../Content"
// #define COMMON_FOLDER "/home/vislab/Gallery/common_content"
#define COMMON_FOLDER "../common_content"

void sigAbrtHandler(int sig);
char* read_file(FILE* file_ptr, char* file_name, int file_size);
int get_file_size(FILE* file_ptr, char* file_name);
_Bool file_exists(char* file_name, char* dir);
char* strtrim(char* str);
void add_to_gallery(FILE* file_ptr, char* new_name, char* old_name, char* desc);
char* print_format(char* str1, char* str2, char* str3, int size);
void make_symbolic_link(pid_t pid, char* contents, char* new_name, char* old_name);
int sym_link_valid(FILE* file_ptr, pid_t pid, int* p1, char* old_name, int file_size);
void change_text_color(char* color);

/* Handle abort errors when signaled */
void sigAbrtHandler(int sig) {
    if(sig == SIGABRT)
        printf("Don't know why this happened, but program still worked lol.\n");
    else if(sig == SIGSEGV) {
        printf("\"Gallery_Add_Multiple.txt\" is probably empty.\n");
        exit(0);
    } 
}

char* read_file(FILE* file_ptr, char* file_name, int file_size) {
    char* new_buff;
    new_buff = malloc((file_size + 1) * sizeof(char));
    file_ptr = fopen(file_name, "rw");
    fseek(file_ptr, 0, SEEK_SET);
    fread(new_buff, file_size, 1, file_ptr); // read contents of file
    fclose(file_ptr);
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
    FILE* file_ptr;
    char* abs_file_name = malloc((strlen(dir) + strlen(file_name) + 2) * sizeof(char));
    strcpy(abs_file_name, "");
    strcat(abs_file_name, dir);
    strcat(abs_file_name, "/");
    strcat(abs_file_name, file_name);
    if(access(abs_file_name, F_OK) != -1) { // fread
        //fclose(abs_file_name);
        //free(abs_file_name);
        return TRUE;
    }
    //free(abs_file_name);
    return FALSE;
}

char* strtrim(char* str) {

    signal(SIGSEGV, sigAbrtHandler);

    int curr_r = strlen(str);
    // replace error chars with spaces
    for(int idx = 0; idx < curr_r; idx++) {
        if(str[idx] < 32 || str[idx] > 122) {
            if(str[idx] != '\n')
                str[idx] = ' ';
        }
    }
    // remove spaces in edges
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
    
    signal(SIGABRT, sigAbrtHandler);

    if(old_name == NULL)
        printf("\n%s <- %s\n", new_name, desc);
    else
        printf("\n%s <- %s, %s\n", new_name, old_name, desc);

    /* add string to 'Gallery_Content.txt' */
    //file_ptr = fopen(GALLERY_NAME, "a");
    if(file_ptr != NULL) {
        //fputs(str_add, file_ptr);
        if(old_name == NULL)
            fprintf(file_ptr, "\n%s <- %s\n", new_name, desc);
        else
            fprintf(file_ptr, "\n%s <- %s, %s\n", new_name, old_name, desc);
        //fclose(file_ptr);
    }
    else
        printf("Could not open \"Gallery_Content.txt\"");
}

char* print_format(char* str1, char* str2, char* str3, int size) {
    char* str = malloc(size * sizeof(char));
    strcpy(str, "");
    strcpy(str, "\n");
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

    signal(SIGABRT, sigAbrtHandler);

    char* new_abs_name = malloc((strlen(CONTENTS_FOLDER) + strlen(new_name) + 2) * sizeof(char));
    char* old_abs_name = malloc((strlen(COMMON_FOLDER) + strlen(old_name) + 2) * sizeof(char));
    strcpy(new_abs_name, "");
    strcat(new_abs_name, CONTENTS_FOLDER);
    strcat(new_abs_name, "/");
    strcat(new_abs_name, new_name);
    strcpy(old_abs_name, "");
    strcat(old_abs_name, COMMON_FOLDER);
    strcat(old_abs_name, "/");
    strcat(old_abs_name, old_name);
	if((pid = fork()) == -1)
		exit(-1);
	else if(pid == 0)
		execlp("ln", "ln", "-s", old_abs_name, new_abs_name, (char*) NULL);
}

int sym_link_valid(FILE* file_ptr, pid_t pid, int* p, char* old_name, int file_size) {
   
    signal(SIGABRT, sigAbrtHandler);
 
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
