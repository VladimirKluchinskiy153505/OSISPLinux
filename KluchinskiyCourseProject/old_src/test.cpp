
#include <unistd.h>
#include <fstream>
#include<chrono>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <iostream>
#include <sstream>
#include <ctime> // Для std::strftime
#include <iomanip>
#include <filesystem>
#include <pqxx/pqxx>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <cstring>
#include "../headers/newnotify.h"
std::string OUTPUT_FILENAME = "/home/vadimir_kluchinskiy/Desktop/CourseExamples/KluchinskiyCourseProject/etc/scan_result.txt";
unsigned long long int MAX_MEMORY_SIZE= 1000*1024;
namespace fs = std::filesystem;
void truncateTable(const std::string& tableName) {
    try {
        pqxx::connection con("dbname=filesaudit user=admin password=123 hostaddr=127.0.0.1 port=5432");
        if (!con.is_open()) {
            std::cerr << "Cannot open database\n";
            return;
        }

        pqxx::work tx(con);
        tx.exec("TRUNCATE " + tableName + " CASCADE;");
        tx.commit();

        std::cout << "Table " << tableName << " truncated successfully.\n";
    } catch (const std::exception& e) {
        syslog(LOG_INFO, (std::string("EXCEPTION !!!")+e.what()).c_str());
        std::cerr << "Error truncating table: " << e.what() << std::endl;
    }
}
std::string get_current_time1() {
    const auto timestamp = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
std::string calculate_checksum_sha256_lowlevel(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Cannot open file: " << path << std::endl;
        return "";
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        EVP_DigestUpdate(ctx, buffer, bytes_read);
    }

    EVP_DigestFinal_ex(ctx, hash, nullptr);
    EVP_MD_CTX_free(ctx);

    close(fd);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}
std::string get_last_modified_time(const fs::path& path) {
    const auto last_modified_time = fs::last_write_time(path);
    std::time_t last_modified = std::chrono::system_clock::to_time_t(
            std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    last_modified_time - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
            )
    );
    std::stringstream last_modified_ss;
    last_modified_ss << std::put_time(std::localtime(&last_modified), "%Y-%m-%d %H:%M:%S");
    return last_modified_ss.str();
}
std::string get_size(const fs::path& path){
    if (fs::is_regular_file(path)) {
        return std::to_string(fs::file_size(path));
        //signature =  std::to_string(calculate_signature(path));
    }else{
        return "-1";
    }
}
int insert_action(const fs::path& path, std::string& action_name,const uint32_t& cookie, bool move_from=false){
    try {
        std::string size= "0";
        std::string timestamp = get_current_time1();
        std::string last_modified_time = timestamp;
        std::string signature  = std::to_string(cookie);
        if(move_from){

        }else{
            size= get_size(path);
            last_modified_time = get_last_modified_time(path);
        }


        std::ofstream file(OUTPUT_FILENAME, std::ios_base::app);
        {
            file << " Path: " << path
                 << " Size: " << size << " bytes"
                 << " Last Modified: " << last_modified_time
                 << " Signature: " << signature
                 << " Timestamp: " << timestamp
                 << " ActionName " << action_name<< std::endl;
        }
        file.close();
        pqxx::connection con("dbname=filesaudit user=admin password=123 hostaddr=127.0.0.1 port=5432");
        if (!con.is_open()) {
            std::cerr << "Cannot open database\n";
            return 1;
        }

        pqxx::work tx(con);
        //tx.exec_prepared("insert_file_data", path.string(), std::filesystem::file_size(path), ss.str(), signature);
        tx.exec(
                "INSERT INTO FilesActionData (path, size, last_modified, signature, action_name, timestamp)"
                "VALUES ('"
                + std::string(path) + "', '"
                + size + "', '"
                + last_modified_time + "', '"
                + signature + "', '"
                + action_name + "', '"
                + timestamp + "')"
        );
        //syslog(LOG_INFO, res.query().c_str());
        tx.commit();
    } catch (const std::exception& e) {
        syslog(LOG_INFO, "%s", (std::string("EXCEPTION !!!")+e.what()).c_str());
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <dir>\n", argv[0]);
        exit(1);
    }

    long page_size = sysconf(_SC_PAGESIZE);
    size_t path_to_statm_size = 64;
    char path_to_statm[path_to_statm_size];
    snprintf(path_to_statm, path_to_statm_size, "/proc/%d/statm", getpid());
    const long max_memory_pages = MAX_MEMORY_SIZE / page_size;

    char* dirs[2] = {argv[1], NULL};
    printf("Start to watch %s page_size=%ld max_memory_pages=%ld\n", dirs[0], page_size, max_memory_pages);

    uint32_t mask = IN_ALL_EVENTS;
    //uint32_t mask = IN_MODIFY | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;

    Notify* ntf = initNotify(dirs, mask, "^\\.");
    if (ntf == NULL)
    {
        exit(EXIT_FAILURE);
    }
    std::string prev_path = "0";
    uint32_t prev_mask = 0;

    for (;;)
    {
        char* path = NULL;
        uint32_t mask = 0;
        uint32_t cookie = 0;
        if (-1 == waitNotify(ntf, &path, &mask, 0, &cookie))
        {
            if (errno == ENOSPC)
            {
                printf("ERROR: The user limit on the total number of inotify watches was reached or the kernel failed to allocate a needed resource.\n");
            }
            else
            {
                printf("ERROR:%s:%d: %s\n", __FILE__, __LINE__, strerror(errno));
            }
            exit(EXIT_FAILURE);
        }
        if(prev_path == std::string(path) && mask == prev_mask){
            continue;
        }
        prev_path = std::string(path);
        prev_mask = mask;
        // std::cout<<get_last_modified_time(path)<<'\n';
        if (path == NULL)
        {
            printf("path=NULL\n");
            exit(EXIT_FAILURE);
        }
        if (mask & IN_ACCESS)
        {
            std::string action_name ="File was accessed.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("File was accessed. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_MODIFY)
        {
            std::string action_name ="File was modified.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("File was modified. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_ATTRIB)
        {
            std::string action_name ="Metadata changed.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("Metadata changed. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_CLOSE_WRITE)
        {
            std::string action_name ="Writtable file was closed.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("Writtable file was closed. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_CLOSE_NOWRITE)
        {
            std::string action_name ="Unwrittable file closed.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("Unwrittable file closed.\t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_OPEN)
        {
            std::string action_name ="File was opened.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("File was opened.  \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_MOVED_FROM)
        {
            std::string action_name ="File was moved from "+std::string(path);
            int exit_code = insert_action(path,action_name,cookie,true);
            printf("File was moved from \t%s. cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_MOVED_TO)
        {
            std::string action_name ="File was moved to "+std::string(path);
            int exit_code = insert_action(path,action_name,cookie);
            printf("File was moved to \t%s. cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_CREATE)
        {
            std::string action_name ="Subfile was created.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("Subfile was created. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_DELETE)
        {
            std::string action_name ="Subfile was deleted.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("Subfile was deleted. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_DELETE_SELF)
        {
            std::string action_name ="Self was deleted.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("Self was deleted.%s\n", path);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_MOVE_SELF)
        {
            std::string action_name = "Self was moved.";
            int exit_code = insert_action(path,action_name,cookie);
            printf("Self was moved. %s\n", path);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_Q_OVERFLOW)
        {
            printf("Event queued overflowed.\n");
            exit(-1);
        }
        if (!strlen(path))
        {
            printf("path=0\n");
            exit(-1);
        }
        /*
        if (strstr(path, "000000"))
        {
            free(path);
            break;
        }
        */
        free(path);
    }

    freeNotify(ntf);

    return 0;
}

