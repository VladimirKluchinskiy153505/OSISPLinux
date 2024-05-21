#ifndef LABWORK5_MERGE_SORT_H
#define LABWORK5_MERGE_SORT_H
#include <iostream>
#include <thread>
#include<chrono>
#include<vector>
#include<deque>
#include<fstream>
#include <random>
using namespace std;
struct merge_structure{
    std::vector<int>& arr;
    int left, middle, right;
};
void display_vec(vector<int>& vec);
int write_random_nums_to_file(const char* filename, size_t num);
void read_numbers(vector<int>& numbers, const char* filename);
int write_numbers(vector<int>& numbers, const char* filename);
void merge2(merge_structure&& ms);
void merge_sort(std::vector<int>& arr, int left, int right);
void async_merge_sort42(vector<int>& array, size_t num_intervals);
#endif //LABWORK5_MERGE_SORT_H
