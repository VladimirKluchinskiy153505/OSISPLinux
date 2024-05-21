#ifndef LABWORK4_FILE_SCAN_LIB_CPP
#define LABWORK4_FILE_SCAN_LIB_CPP
#include "../headers/file_scan_lib.h"
std::string OUTPUT_FILENAME = "//home/vadimir_kluchinskiy/Desktop/ClionProjects/LabWork4/etc/scan_file_result.txt";
std::mutex mtx;
std::condition_variable cv;
std::queue<fs::path> file_queue;
std::atomic<bool> stop_flag(false);
std::mutex file_mtx;
std::uint64_t calculate_signature(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
    std::uint64_t signature = 0;
    for (const auto& byte : buffer) {
        signature ^= static_cast<std::uint64_t>(byte);
    }

    return signature;
}

void worker_thread(int thread_id) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !file_queue.empty() || stop_flag; });

        if (stop_flag && file_queue.empty()) {
            return;
        }

        const auto path = file_queue.front();
        file_queue.pop();
        lock.unlock();

        try {
            const auto signature = calculate_signature(path);
            const auto timestamp = std::chrono::system_clock::now();
            std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
            std::fstream file(OUTPUT_FILENAME, std::fstream::in| std::fstream::out|std::fstream ::app);
            {
                std::lock_guard<std::mutex> lock(file_mtx);
                file << "Thread " << thread_id << " Path: " << path.string() << " Signature: " << signature << " Timestamp: " << std::ctime(&time)<< std::endl;
            }
            file.close();
        } catch (const std::exception& e) {
            std::cerr << "Error in thread " << thread_id << ": " << e.what() << std::endl;
        }
    }
}

int scan_files(std::vector<std::string>& root_directories, std::string output_filename){
    try {

        if(!output_filename.empty()){
            OUTPUT_FILENAME = output_filename;
        }
        //OUTPUT_FILENAME = output_filename;
       // const fs::path root_path = root_filename;
        const int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        std::fstream output_file(OUTPUT_FILENAME , std::fstream::in| std::fstream::out|std::fstream::trunc);
        output_file.close();


        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(worker_thread, i);
        }
        for(std::string& root_path: root_directories){
            for (const auto& entry : fs::recursive_directory_iterator(root_path)) {
                if (entry.is_regular_file()) {
                    file_queue.push(entry.path());
                }
            }
        }

        stop_flag = true;
        cv.notify_all();

        for (auto& thread : threads) {
            thread.join();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
#endif