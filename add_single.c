#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include "add_file.h"

// #define GALLERY_ADD "Gallery_Add_Single.txt"
#define GALLERY_ADD "/mnt/c/Users/jan/Desktop/Files/Work_Material/Employment/Visualizations_Lab/Display_Cluster/Stallion_File_Finder/Mock_Gallery_Add_Single.txt"

int find_file_contents(int file_size);
int get_c_idx(int file_size, int c, int* find_mode);
void reset_gallery_add(FILE* file_ptr);

FILE *gallery;
char* buff;
char* new_name;
char* old_name;
char* desc;
int file_size;
int p1[2];
int p2[2];
pid_t pid;
pid_t cpid;

int main(int argc, char* argv[]) {

    /* Handler any abort errors */
    signal(SIGABRT, sigAbrtHandler);

    /* File variables */
    int new_exists = 0;
    int old_exists = 1;
    int prev_sym_links = 0;

    /* if parameters are given. */
    if(argc > 1) {

        /* if new name, old name, and description are given as parameters. */
        if(argc == 4) {
            // get new name
            int new_name_size = strlen(argv[1]);
            new_name = malloc((new_name_size + 1) * sizeof(char));
            strncpy(new_name, argv[1], new_name_size);

            // get old name
            int old_name_size = strlen(argv[2]);
            old_name = malloc((old_name_size + 1) * sizeof(char));
            strncpy(old_name, argv[2], old_name_size);
            if(strstr(old_name, ".") == NULL) {

                // print error message
                change_text_color(RED);
                printf("ERROR: \"%s\" isn't a proper file name. It must be type \".png\", \".jpg\", \".mov\", etc...\n\n", old_name);
                change_text_color(DEFAULT);

                // free memory
                free(new_name);
                free(old_name);

                // return error
                return -1;
            }

            // get description
            int desc_size = strlen(argv[3]);
            desc = malloc((desc_size + 1) * sizeof(char));
            strncpy(desc, argv[3], desc_size);

            // make symbolic link and add it to content
            if(!(prev_sym_links = sym_link_valid(gallery, pid, p1, old_name, file_size))
                && (old_exists = file_exists(old_name, COMMON_FOLDER))
                && !(new_exists = file_exists(new_name, CONTENTS_FOLDER))) {
                    FILE *file_ptr = fopen(GALLERY_NAME, "a");
                    add_to_gallery(file_ptr, new_name, old_name, desc);
                    fclose(file_ptr);
                    if((old_name != NULL) && (strlen(old_name) > 0))
                        make_symbolic_link(pid, CONTENTS_FOLDER, new_name, old_name);
            }
            else { /* Output details on why symbolic link was NOT possible */
                if(!old_exists)
                    printf("File name \"%s\" doesn't exist in current working directory. Can't create symbolic link\n", old_name);
                if(new_exists) {
                    printf("Symbolic link name \"%s\" already exists.\n", new_name);
                    return -1;
                }
                if(prev_sym_links)
                    printf("File name \"%s\" already has a symbolic link.\n", old_name);
            }

            /* free memory */
            free(buff);
            free(new_name);
            free(old_name);
            free(desc);

            return 0;
        }
        else
            printf("ERROR: incorrect format. Try ./add_mult NEW_NAME OLD_NAME \"KEYWORD_1, KEYWORD_2, ...\"");
    }

    /* if no parameters are given (and "Gallery_Add_Single.txt" is filled out) */

    int file_size = get_file_size(gallery, GALLERY_ADD);
    buff = read_file(gallery, GALLERY_ADD, file_size);

    /* Determine whether symbolic link is possible */
    if((find_file_contents(file_size) > 0) && !(prev_sym_links = sym_link_valid(gallery, pid, p1, old_name, file_size))
                                           && (old_exists = file_exists(old_name, COMMON_FOLDER))
                                           && !(new_exists = file_exists(new_name, CONTENTS_FOLDER))) {
        FILE *file_ptr = fopen(GALLERY_NAME, "a");
        add_to_gallery(file_ptr, new_name, old_name, desc);
        fclose(file_ptr);
        if((old_name != NULL) && (strlen(old_name) > 0))
            make_symbolic_link(pid, CONTENTS_FOLDER, new_name, old_name);
        file_ptr = fopen(GALLERY_ADD, "w+");
        reset_gallery_add(file_ptr);
        fclose(file_ptr);
    }
    else { /* Output details on why symbolic link was NOT possible */
        if(!old_exists)
            printf("File name \"%s\" doesn't exist in current working directory. Can't create symbolic link\n", old_name);
        if(new_exists) {
            printf("Symbolic link name \"%s\" already exists.\n", new_name);
            /* free memory */
            free(buff);
            free(new_name);
            free(old_name);
            free(desc);

            return -1;
        }
        if(prev_sym_links)
            printf("File name \"%s\" already has a symbolic link.\n", old_name);
    }

    /* free memory */
    free(buff);
    free(new_name);
    free(old_name);
    free(desc);

    return 0;
}

int find_file_contents(int file_size) {
    int total_chars_read = 0;
    int find_mode = -1;
    int prev_mode = -1;
    int buff_idx = 0;
    for(int c = 0; (c < file_size) && (find_mode < 3); c++) {
        c = get_c_idx(file_size, c, &find_mode);
        if(prev_mode != -1) {
            if(buff[c] == '\n')
                buff[c] = ' ';
            buff[buff_idx] = buff[c];
            buff_idx++;
            if(prev_mode != find_mode) {
                buff[buff_idx - 1] = '\000';
                if(prev_mode == 0) {
                    new_name = malloc(buff_idx * sizeof(char));
                    strncpy(new_name, buff, buff_idx);
                    strcpy(new_name, strtrim(new_name));
                    // printf("|%s|\n", new_name);
                }
                else if(prev_mode == 1) {
                    old_name = malloc(buff_idx * sizeof(char));
                    strncpy(old_name, buff, buff_idx);
                    strcpy(old_name, strtrim(old_name));
                    // printf("|%s|\n", old_name);
                }
                else if(prev_mode == 2) {
                    desc = malloc(buff_idx * sizeof(char));
                    strncpy(desc, buff, buff_idx);
                    strcpy(desc, strtrim(desc));
                    // printf("|%s|\n", desc);
                }
                buff_idx = 0;
            }
        }
        prev_mode = find_mode;
    }

    if((new_name == NULL) || (old_name == NULL) || (desc == NULL))
        return 0;
    else if(strlen(new_name) == 0) {
        printf("\"New Name\" field is empty. No file can be created without a proper new name.\n");
        return 0;
    }
    else if(strlen(desc) == 0) {
        printf("\"Desc\" field is empty. Please add a proper description and keywords for future reference.\n");
        return 0;
    }

    return strlen(new_name) + strlen(old_name) + strlen(desc);
}

int get_c_idx(int file_size, int c, int* find_mode) {
    if((c < file_size - 11) && buff[c] == '\n' && buff[c+1] == '>' && buff[c+3] == 'N' && buff[c+4] == 'e' && buff[c+5] == 'w' && buff[c+6] == ' ' && buff[c+7] == 'N' && buff[c+8] == 'a' && buff[c+9] == 'm' && buff[c+10] == 'e' && buff[c+11] == ':') {
        c += 35;
        *find_mode = 0;
    }
    else if((c < file_size - 11) && buff[c] == '\n' && buff[c+1] == '>' && buff[c+3] == 'O' && buff[c+4] == 'l' && buff[c+5] == 'd' && buff[c+6] == ' ' && buff[c+7] == 'N' && buff[c+8] == 'a' && buff[c+9] == 'm' && buff[c+10] == 'e' && buff[c+11] == ':') {
        c += 35;
        *find_mode = 1;
    }
    else if((c < file_size - 16)  && buff[c] == '\n' && buff[c+1] == '>' && buff[c+3] == 'D' && buff[c+4] == 'e' && buff[c+5] == 's' && buff[c+6] == 'c' && buff[c+7] == '/' && buff[c+8] == 'K' && buff[c+9] == 'e' && buff[c+10] == 'y' && buff[c+11] == 'w' && buff[c+12] == 'o' && buff[c+13] == 'r' && buff[c+14] == 'd' && buff[c+15] == 's' && buff[c+16] == ':') {
        c += 40;
        *find_mode = 2;
    }
    else if((c < file_size - 23) && buff[c] == '\n' && buff[c+1] == '>' && buff[c+3] == 'E' && buff[c+4] == 'n' && buff[c+5] == 'd' && buff[c+7] == '#' && buff[c+8] == ' ' && buff[c+9] == 'l' && buff[c+10] == 'e' && buff[c+11] == 'a' && buff[c+12] == 'v' && buff[c+13] == 'e' && buff[c+14] == ' '  && buff[c+15] == 't'  && buff[c+16] == 'h' && buff[c+17] == 'i' && buff[c+18] == 's' && buff[c+19] == ' ' && buff[c+20] == 'h' && buff[c+21] == 'e' && buff[c+22] == 'r' && buff[c+23] == 'e')
        *find_mode = 3;
    return c;
}

void reset_gallery_add(FILE* file_ptr) {
    
    signal(SIGABRT, sigAbrtHandler);

    char line_0[] = "Fill in the file information with the corresponding new name, old name, and description/keywords.";
    char line_1[] = "Don't erase the end command, i.e. leave \"End # leave this here\" as it is.";
    char line_2[] = "> New Name: # Write under this line";
    char line_3[] = "> Old Name: # Write under this line";
    char line_4[] = "> Desc/Keywords: # Write under this line";
    char line_5[] = "> End # leave this here";
    if(file_ptr != NULL)
        fprintf(file_ptr, "%s\n%s\n\n%s\n\n%s\n\n%s\n\n%s", line_0, line_1, line_2, line_3, line_4, line_5);
}

