#include "../headers/censor_text_lib.h"
void clean_file_sources(FILE* input_file, FILE* output_file){
    //free(PairArray);
    fclose(input_file);
    fclose(output_file);
}
void writeStringToFile(FILE* file, const char* str) {
    fprintf(file, "%s\n", str);
}
void replaceWord(char* str, const char* oldW, const char* newW)
{
    char* result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);

    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], oldW) == &str[i]) {
            cnt++;
            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }
    // Making new string of enough length
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1);
    i = 0;
    char *orig_str = str;
    while (*str) {
        // compare the substring with the result

        if (strstr(str, oldW) == str) {
            strcpy(&result[i], newW);
            i += newWlen;
            str += oldWlen;
        }
        else
            result[i++] = *str++;
    }
    result[i] = '\0';
    str = orig_str;
    i=0;
    for (; result[i] != '\0'; ++i) {
        str[i] = result[i];
    }
    str[i] = '\0';
    free(result);
}
void censor_text(struct Pair* PairArray, int* array_size, const char* input_filename, const char* output_filename) {
    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        printf("Failed to open input_file\n");
        return;
    }
    FILE* output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("Failed to open output_file\n");
        return;
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, input_file)) {
        line[strcspn(line, "\n")] = '\0';
        printf("SourceLine: %s\n", line);
        for(int i=0;i<*array_size;++i){
            replaceWord(line, PairArray[i].first, PairArray[i].second);
        }
        writeStringToFile(output_file, line);
        printf("Corrected_line: %s\n", line);
        //printf("%s\n", line);
    }
    clean_file_sources(input_file, output_file);
}
