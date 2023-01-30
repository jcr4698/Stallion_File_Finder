#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include "../add_file.h"

// #define GALLERY_ADD "Gallery_Add_Single.txt"
#define GALLERY_ADD "/mnt/c/Users/jan/Desktop/Files/Work_Material/Employment/Visualizations_Lab/Display_Cluster/Stallion_File_Finder/Mock_Gallery_Add_Single.txt"

int find_file_contents(int file_size);
int get_c_idx(int file_size, int c, int* find_mode);
void reset_gallery_add(FILE* file_ptr);

FILE *gallery;
char* buff;
int file_size;
int p1[2];
int p2[2];
pid_t pid;
pid_t cpid;

int add_file(char* new_name, char* old_name, char* desc) { // recreate the add_single as an H_FILE so that you can use it in the GUI (file_finder.c)

    /* Handler any abort errors */
    signal(SIGABRT, sigAbrtHandler);

    /* File variables */
    int new_exists = 0;
    int old_exists = 1;
    int prev_sym_links = 0;

    if(new_name == NULL || old_name == NULL || desc == NULL)
        return -1;

    /* Check whether inputs are valid*/
    if((new_name != NULL) && (strlen(strtrim(new_name)) < 1)) {
        printf("ERROR: incorrect format. one or more fields is empty/missing.\n");
        return -1;  // empty string
    }
    if((old_name != NULL) && (strlen(strtrim(old_name)) < 1)) {
        printf("ERROR: incorrect format. one or more fields is empty/missing.\n");
        return -1;  // empty string
    }
    if((desc != NULL) && (strlen(strtrim(desc)) < 1)) {
        printf("ERROR: incorrect format. one or more fields is empty/missing.\n");
        return -1;  // empty string
    }
    if(strstr(old_name, ".") == NULL) {

        // print error message
        change_text_color(RED);
        printf("ERROR: \"%s\" isn't a proper file name. It must be type \".png\", \".jpg\", \".mov\", etc...\n", old_name);
        change_text_color(DEFAULT);

        // return error
        return -1;  // wrong string
    }

    /* Make symbolic link and add it to content */
    if(!(prev_sym_links = sym_link_valid(gallery, pid, p1, old_name, file_size))
        && (old_exists = file_exists(old_name, COMMON_FOLDER))
        && !(new_exists = file_exists(new_name, CONTENTS_FOLDER))) {
            FILE *file_ptr = fopen("./Mock_Gallery_Content.txt", "a");
            add_to_gallery(file_ptr, new_name, old_name, desc);
            fclose(file_ptr);
            if((old_name != NULL) && (strlen(old_name) > 0))
                make_symbolic_link(pid, CONTENTS_FOLDER, new_name, old_name);
    }
    else { /* Output details on why symbolic link was NOT possible */
        if(prev_sym_links) { // symbolic link already exists
            printf("File name \"%s\" already has a symbolic link.\n", old_name);
            return -6;  // delete all of them
        }
        int err = 0; // existence error
        if(!old_exists) {
            printf("File name \"%s\" doesn't exist in current working directory. Can't create symbolic link\n", old_name);
            err -= 2;
        }
        if(new_exists) {
            printf("Symbolic link name \"%s\" already exists.\n", new_name);
            err -= 3;
        }
        return err;
    }

    return 0;
}
