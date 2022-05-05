
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* gallery;

char lget(char c);
char* filter(char c);
int old_name(char c, char** save_word, char** word);
int new_name(char c, char** save_word, char** word);
int keys(char c, char** save_word, char** word);

int main(int argc, char* argv[]) {
    gallery = fopen("Gallery_Content.txt", "r");

    /* For each character */
    char c;
    //int char_size = 0;
    //int file_name = 0;
    //int key_words = 0;
    char* save_word = malloc(sizeof(char));

    int sec = 0;

    c = ' ';
    while(c != EOF) {
        char* word = malloc((strlen(save_word))*sizeof(char));
        while((c != EOF) && (sec == 0)) {
	    word = malloc((strlen(save_word))*sizeof(char));
	    strcpy(word, save_word);
            c = lget(fgetc(gallery));
            sec = new_name(c, &save_word, &word);
	    free(word);
	}
	printf("%s\n", save_word);
	free(save_word);
	save_word = malloc(sizeof(char));
	strcpy(save_word, "");
	while((c != EOF) && (sec == 1)) {
            word = malloc((strlen(save_word))*sizeof(char));
            strcpy(word, save_word);
            c = lget(fgetc(gallery));
            sec = old_name(c, &save_word, &word);
	    free(word);
        }
	printf("%s\n", save_word);
	free(save_word);
	save_word = malloc(sizeof(char));
	strcpy(save_word, "");
	while((c != EOF) && (sec == 2)) {
            word = malloc((strlen(save_word))*sizeof(char));
	    strcpy(word, save_word);
	    c = lget(fgetc(gallery));
	    sec = keys(c, &save_word, &word);
	    if((c != EOF) && (sec > 2)) {
                 printf("%s\n", save_word);
		 free(save_word);
		 if(sec == 3) {
                     save_word = malloc(sizeof(char));
		     strcpy(save_word, "");
                     sec = 2;
		 }
	    }
	    free(word);
        }
	save_word = malloc(sizeof(char));
	strcpy(save_word, "");
	fgetc(gallery); // extra '\n'
	sec = 0;
    }

    fclose(gallery);
}

int old_name(char c, char** save_word, char** word) {
    if(c != ' ' && c != '-' && c != '\n') {
	if(c != '<') {
            free(*save_word);
            *save_word = malloc((strlen((const char*) *word) + 1)*sizeof(char));
            strcpy(*save_word, strcat((char*) *word, &c));
	}
        //*save_word[strlen((const char*) *save_word)-1] = '\00';
    }
    if(c == '\n') {
        //printf("%s |\n", *word); // add 'word' to hash as orig_name
        //free(*word);
        //*save_word = malloc(sizeof(char));
        return 2;
    }
    return 1;
}

int new_name(char c, char** save_word, char** word) {
    if(c != '<' && c != ' ' && c != '\n') {
        free(*save_word);
        *save_word = malloc((strlen((const char*) *word) + 1)*sizeof(char));
        strcpy(*save_word, strcat((char*) *word, &c));
	return 0;
    }
    return 1;
}

int keys(char c, char** save_word, char** word) {
    if(c != ',' && c != '\n') {
	//*save_word = malloc(sizeof(char));
	if(c != ':') {
            free(*save_word);
	    *save_word = malloc((strlen((const char*) *word) + 1)*sizeof(char));
	    strcpy(*save_word, strcat((char*) *word, &c));
	}
        else {
            free(*save_word);
            *save_word = malloc(sizeof(char));
        }
	return 2;
    }
    else if(c == ',')
        return 3;
    return 4;
}

char lget(char c) {
    if(c > 96 && c < 123)
        return c - 32;
    return c;
}

char* filter(char c) {
    if(c == ',')
        return "\n";
    else if(c == '\n')
        return "\n--\n";
    else if(c == ' ')
        return "";
    else {
        char* s = malloc(sizeof(char));
	s[0] = c;
        return s;
    }
}

void form_word(char* word, int size) {
    char* new_word = malloc((size-1)*sizeof(char));
    
}
