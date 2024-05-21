#ifndef EXTRACT_DICTIONARY
#define EXTRACT_DICTIONARY
#include "../headers/extract_dictionary_lib.h"

void clean_sources(FILE* file){
    //free(PairArray);
    fclose(file);
}
struct Pair* extract_text(const char* filename, int* array_size0) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return NULL;
    }
    int array_size = 0;
    struct Pair* PairArray = malloc(sizeof(struct Pair)*MAX_ARRAY_LENGTH);

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        line[strcspn(line, "\n")] = '\0';
        //printf("%s\n", line);
        char* token = strtok(line, ":");
        //printf("FirstToken %s\n", token);
        if (token) {
            strncpy(PairArray[array_size].first, token, MAX_WORD_LENGTH);
            token = strtok(NULL, ":");
            //printf("SecondToken %s\n", token);
            if (token) {
                strncpy(PairArray[array_size].second, token, MAX_WORD_LENGTH);

                ++array_size;

            } else {
                printf("Word has no translation\n");
                clean_sources(file);
                return NULL;
            }
        } else {
            printf("Line has no word\n");
            clean_sources(file);
            return PairArray;
        }
    }
    clean_sources(file);
    *array_size0 = array_size;
    return PairArray;
}
#endif