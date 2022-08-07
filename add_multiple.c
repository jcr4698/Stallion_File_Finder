#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "add_file.h"

#define GALLERY_ADD "Gallery_Add_Multiple.txt"

int next_line(int curr_idx, char* temp_buff);
int find_file_contents(char* curr_buff);
void failed_lines(FILE* file_ptr, char* failed_buff);
void reset_gallery_add(FILE* file_ptr);

FILE *gallery;
char* buff;
char* new_name;
char* old_name;
char* desc;
int p1[2];
int p2[2];
pid_t pid;
pid_t cpid;

int main(int argc, char* argv[]) {
    int new_exists = 0;
    int old_exists = 1;
    int prev_sym_links = 0;
    int file_size = get_file_size(gallery, GALLERY_ADD);
    buff = read_file(gallery, GALLERY_ADD, file_size);
    
    /* scan each line of the file */
    char* curr_buff = malloc((file_size + 1) * sizeof(char));
    reset_gallery_add(gallery);
    for(int curr_ch = 0; curr_ch < file_size; curr_ch++) {
        curr_ch = next_line(curr_ch, curr_buff);
        strcpy(curr_buff, strtrim(curr_buff));
        if(strlen(curr_buff) > 0) {
            if(find_file_contents(curr_buff) > 0) {
                if(!(prev_sym_links = sym_link_valid(gallery, pid, p1, old_name, file_size))
                    && (old_exists = file_exists(old_name, COMMON_FOLDER))
                    && !(new_exists = file_exists(new_name, CONTENTS_FOLDER))) {
                    add_to_gallery(gallery, new_name, old_name, desc);
                    make_symbolic_link(pid, CONTENTS_FOLDER, new_name, old_name);
                }
                else {
                    /* print error message */
                    change_text_color(RED);
                    printf("%s:\n", curr_buff);
                    if(!old_exists)
                        printf("File name \"%s\" doesn't exist in current working directory.\n", old_name);
                    if(new_exists)
                        printf("Symbolic link for \"%s\" already exists.\n", new_name);
                    if(prev_sym_links)
                        printf("File name \"%s\" already has a symbolic link.\n", old_name);
                    printf("\n");
                    change_text_color(DEFAULT);

                    /* save rejected lines */
                    failed_lines(gallery, curr_buff);
                    
                }
            }
            else    /* save rejected lines */
                failed_lines(gallery, curr_buff);
        }
    }
    failed_lines(gallery, "\n");
    return 0;
}

void failed_lines(FILE* file_ptr, char* failed_buff) {
    file_ptr = fopen(GALLERY_ADD, "a");
    if(file_ptr == NULL)
        printf("Error opening file.\n");
    if((failed_buff != NULL) && (strlen(strtrim(failed_buff)) > 0)) {
        fputs("\n\n", file_ptr);
        fputs(failed_buff, file_ptr);
    }
    fclose(file_ptr);
}

int next_line(int curr_idx, char* temp_buff) {
    strcpy(temp_buff, "");
    int buff_idx = 0;
    int temp_idx = curr_idx;
    int cmd = 1;
    char ch = ' ';
    int size = strlen(buff);
    for(int c = curr_idx; ((ch = buff[c]) != '\n') && (ch != EOF) && (buff_idx < size); c++) {
        temp_idx++;
        if(cmd) {
            if(ch == '#')
                cmd = 0;
            else
                temp_buff[buff_idx++] = ch;
        }
    }
    temp_buff[buff_idx] = '\000';
    return temp_idx;
}

int find_file_contents(char* curr_buff) {
    int size = strlen(curr_buff);
    int find_mode = 0;
    change_text_color(RED);
    if((strstr(curr_buff, "<-") != NULL) && (strstr(curr_buff, ",") != NULL)) {
        char* buff = malloc((size + 1) * sizeof(char));
        int buff_idx = 0;
        char ch;
        for(int c = 0; c < size; c++) {
            ch = curr_buff[c];
            if((ch == '<') && (find_mode == 0)) {
                new_name = malloc(buff_idx * sizeof(char));
                buff[buff_idx] = '\000';
                strcpy(new_name, strtrim(buff));
                // printf("%s\n", new_name);
                buff_idx = 0;
                find_mode++;
                c++;
            }
            else if((ch == ',') && (find_mode == 1)) {
                buff[buff_idx] = '\000';
                old_name = malloc(buff_idx * sizeof(char));
                strcpy(old_name, strtrim(buff));
                // printf("%s\n", old_name);
                buff_idx = 0;
                find_mode++;
            }
            else
                buff[buff_idx++] = ch;
        }
        buff[buff_idx] = '\000';
        desc = malloc(buff_idx * sizeof(char));
        strcpy(desc, strtrim(buff));
        // printf("%s\n", desc);
    }
    else {
        printf("ERROR: Format in \"%s\", line requires both \'<-\' and \',\'.\n\n", curr_buff);
        change_text_color(DEFAULT);
        return 0;
    }

    if((strlen(new_name) == 0) || (strlen(old_name) == 0) || (strlen(desc) == 0)) {
        printf("ERROR: Could not format \"%s\". One or more elements missing in format.\n\n", curr_buff);
        change_text_color(DEFAULT);
        return 0;
    }
    if(strstr(old_name, ".") == NULL) {
        printf("ERROR: \"%s\" isn't a proper file name. It must be type \".png\", \".jpg\", \".mov\", etc...\n\n", old_name);
        change_text_color(DEFAULT);
        return 0;
    }
    change_text_color(DEFAULT);
    return strlen(new_name) + strlen(old_name) + strlen(desc);
}

void reset_gallery_add(FILE* file_ptr) {
    file_ptr = fopen(GALLERY_ADD, "w+");
    char line_0[] = "# Fill out information below in this format (all information for one file must be in one line):\n";
    char line_1[] = "# NEW_FILE_NAME.PNG <- OLD_FILE_NAME.PNG, KEYWORD_1, KEYWORD_2, KEYWORD3,...";
    if(file_ptr != NULL) {
        char* str = malloc((strlen(line_0) + strlen(line_1)) * sizeof(char));
        strcat(str, line_0);
        strcat(str, line_1);
        fputs(str, file_ptr);
        fclose(file_ptr);
    }
}
