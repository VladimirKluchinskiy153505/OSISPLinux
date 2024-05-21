#include "../headers/merge_sort.h"
int main(int argc, char *argv[])
{
    const char* input_filename = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/LabWork5/documents/input_numbers.txt";
    const char* output_filename = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/LabWork5/documents/output_numbers.txt";
    vector<int> vec,vec1;
    size_t  nums_count=10000, threds_number = 8;
    if(argc == 2){
        threds_number = stoul(argv[1]);
    }
    else if (argc == 3) {
        threds_number = stoul(argv[1]);
        nums_count = stoul(argv[2]);
        write_random_nums_to_file(input_filename,nums_count);
    }
    printf("OutputFilename: %s RandomNumbersCount: %zu ThreadsCount %zu\n", output_filename, nums_count, threds_number);
    read_numbers(vec,input_filename);
    read_numbers(vec1,input_filename);
//    printf("Array to sort is:\n");
//    display_vec(vec);

    auto start = chrono::high_resolution_clock::now();
    async_merge_sort42(vec, threds_number);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    // display_vec(vec);

    auto start1 = chrono::high_resolution_clock::now();
    merge_sort(vec1, 0, vec1.size() - 1);
    auto end1 = chrono::high_resolution_clock::now();
    chrono::duration<double> duration1 = end1 - start1;

    //if(vec.size()<100000) {
//        cout << "Result Of AsyncSort\n";
//        display_vec(vec);
    //}

    cout << "Asynchronous Execution Duration " << duration.count() << "Seconds\n";
    cout << "Synchronous Execution Duration " << duration1.count() << "Seconds\n";
    write_numbers(vec, output_filename);
    return 0;
}