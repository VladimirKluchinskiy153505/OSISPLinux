#ifndef PROJECT3_CENSOR_TEXT_LIB_H
#define PROJECT3_CENSOR_TEXT_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structure.h"
void clean_file_sources(FILE* input_file, FILE* output_file);
void writeStringToFile(FILE* file, const char* str);
void replaceWord(char* str, const char* oldW, const char* newW);
void censor_text(struct Pair* PairArray, int* array_size, const char* input_filename, const char* output_filename);
#endif //PROJECT3_CENSOR_TEXT_LIB_H
