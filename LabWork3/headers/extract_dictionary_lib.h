#ifndef PROJECT3_EXTRACT_DICTIONARY_LIB_H
#define PROJECT3_EXTRACT_DICTIONARY_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structure.h"
#define MAX_ARRAY_LENGTH 1000
void clean_sources(FILE* file);
struct Pair* extract_text(const char* filename, int* array_size0);
#endif //PROJECT3_EXTRACT_DICTIONARY_LIB_H
