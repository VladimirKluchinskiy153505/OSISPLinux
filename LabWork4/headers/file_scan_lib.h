#ifndef LABWORK4_FILE_SCAN_LIB_H
#define LABWORK4_FILE_SCAN_LIB_H
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;
std::uint64_t calculate_signature(const fs::path& path);
void worker_thread(int thread_id);
int scan_files(std::vector<std::string>& root_directories, std::string output_filename);
#endif //LABWORK4_FILE_SCAN_LIB_H
