#include "../headers/data_structure.h"
#include "../headers/censor_text_lib.h"
#include "../headers/extract_dictionary_lib.h"
void display_array(struct Pair* PairArray, int* array_size){
    printf("Dictionary of size: %d\n",*array_size);
    for(int i=0;i<*array_size;++i){
        printf("%d)%s : %s\n",i+1,PairArray[i].first,PairArray[i].second);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    const char* input_document = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/Project3/documents/input_text.txt";
    const char* output_document = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/Project3/documents/output_text.txt";
    if (argc > 1) {
        printf("OutputFileName: %s\n", argv[1]);
        output_document = argv[1];
    }
    const char* dictionary_filename = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/Project3/documents/dict_source2.txt";
    int* array_size = calloc(1,sizeof(int));
    struct Pair* PairArray = extract_text(dictionary_filename,array_size);

    display_array(PairArray, array_size);
    censor_text(PairArray, array_size, input_document, output_document);
    free(PairArray);
    free(array_size);
}

