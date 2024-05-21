#include "../headers/file_scan_lib.h"
const char* config_filename = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/LabWork4/etc/config.txt";
const char* output_filename = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/LabWork4/etc/scan_file_result.txt";
int main() {
    std::vector<std::string> root_directories;
    std::string directory;
    std::ifstream config_file(config_filename);

    if (config_file.is_open()) {
        while (getline(config_file, directory)) {
            root_directories.push_back(directory);
        }
        config_file.close();
    } else {
        std::cerr << "Unable to open config";
    }
    scan_files(root_directories, output_filename);
    return 0;
}
