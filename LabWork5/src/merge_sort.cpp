#ifndef LABWORK5_MERGE_SORT_CPP
#define LABWORK5_MERGE_SORT_CPP
#include "../headers/merge_sort.h"

void display_vec(vector<int>& vec) {
    for (auto& now : vec) {
        cout << now << ' ';
    }
    cout << '\n';
}
int write_random_nums_to_file(const char* filename, size_t numbers_count ){
    std::fstream outputFile(filename, ios::trunc|ios::binary|ios::out);

    if (!outputFile.is_open()) {
        std::cerr << "error writing to file" <<'\n';
        return 1;
    }
    vector<int> vec(numbers_count);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(1, numbers_count); // Çäåñü 1 è 1000 - äèàïàçîí ñëó÷àéíûõ ÷èñåë
    vec.resize(numbers_count);
    for (int i = 0; i < numbers_count; ++i) {
        vec[i] = dis(gen);
    }
    for (auto& num: vec) {
        outputFile << num << ' ';
    }
    outputFile.close();
    std::cout << "Succed write" << numbers_count << "intoFile" << filename << "." << std::endl;
    return 0;
}

void read_numbers(vector<int>& numbers, const char* filename) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to read" <<'\n';
        return;
    }
    int number;
    while (inputFile >> number) {
        numbers.push_back(number);
    }
    inputFile.close();
    std::cout << "Read " << numbers.size() << " Numbers from " << filename <<'\n';
}
int write_numbers(vector<int>& numbers, const char* filename){
    std::fstream outputFile(filename, ios::trunc|ios::binary|ios::out);

    if (!outputFile.is_open()) {
        std::cerr << "error writing to file" <<'\n';
        return 1;
    }
    for (auto& num: numbers) {
        outputFile << num << ' ';
    }
    outputFile.close();
    return 0;
}
void merge2(merge_structure&& ms) {
    vector<int>& arr = ms.arr;
    int left = ms.left, middle = ms.middle, right = ms.right;
    int n1 = middle - left + 1, n2 = right - middle;

    vector<int> leftArr(n1);
    vector<int> rightArr(n2);

    for (int i = 0; i < n1; ++i) {
        leftArr[i] = arr[left + i];
    }
    for (int j = 0; j < n2; ++j) {
        rightArr[j] = arr[middle + 1 + j];
    }
    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            arr[k] = leftArr[i];
            ++i;
        }
        else {
            arr[k] = rightArr[j];
            ++j;
        }
        ++k;
    }
    while (i < n1) {
        arr[k] = leftArr[i];
        ++i;
        ++k;
    }
    while (j < n2) {
        arr[k] = rightArr[j];
        ++j;
        ++k;
    }
}
void merge_sort(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        merge_sort(arr, left, middle);
        merge_sort(arr, middle + 1, right);
        merge_structure ms{ref(arr), left, middle, right};
        merge2(merge_structure{ref(arr), left, middle, right});
        //merge2(ms);
    }
}
void async_merge_sort42(vector<int>& array, size_t num_intervals) {
    cout << "Threads count: " << num_intervals << '\n';
    //std::vector<int> array = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }; // Âàø ìàññèâ
    size_t size = array.size(); // Êîëè÷åñòâî èíòåðâàëîâ
    int step = size / num_intervals - 1; // Ðàçìåð êàæäîãî èíòåðâàëà

    vector<pair<int, int>> intervals(num_intervals);
    intervals[0].first = 0, intervals[0].second = step;
    for (int i = 1; i < num_intervals; ++i) {
        intervals[i].first = intervals[i - 1].second + 1;
        intervals[i].second = intervals[i].first + step;
    }
    intervals[num_intervals - 1].second = size - 1;
    cout << "Intervals\n";
    for (auto& now : intervals) {
        cout << now.first << ' ' << now.second << '\n';
    }
    vector<thread> threads(num_intervals);
    for (int i = 0; i < num_intervals; ++i) {
        threads[i] = thread(merge_sort, ref(array), intervals[i].first, intervals[i].second);
    }
    /*for (int i = 0; i < num_intervals; ++i) {
        threads[i].join();
    }*/
    while (intervals.size() >= 2) {
        int count_intervals = intervals.size();
        int count_threads = threads.size();
        vector<pair<int, int>> second_intervals;
        vector<thread> second_threads;
        for (int i = 0; i < count_intervals - 1; i += 2) {//2 transform to one
            threads[i].join();
            if (count_threads >= i + 2) {
                threads[i + 1].join();
            }
            merge_structure ms{ref(array), intervals[i].first, intervals[i].second, intervals[i + 1].second};
            second_threads.push_back(thread(merge2,ms));
            second_intervals.push_back({ intervals[i].first, intervals[i+1].second });
        }
        if (count_intervals & 1) {
            int last = count_intervals - 1;
            if (count_threads >= last+1) {
                threads[last].join();
            }
            second_intervals.push_back(intervals[last]);
        }
        threads.clear();
        threads = std::move(second_threads);
        intervals = second_intervals;
    }
    threads[0].join();
    /*for (auto& now : second_threads) {
        now.join();
    }*/
    /* merge(array, intervals[0].first, intervals[1].second, intervals[3].second);*/
}
#endif //LABWORK5_MERGE_SORT_CPP
