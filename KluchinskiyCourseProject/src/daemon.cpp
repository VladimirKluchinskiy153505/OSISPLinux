#ifndef DAEMON_CPP
#define DAEMON_CPP
#include "../headers/daemon.h"
#include "../headers/newnotify.h"
unsigned long long int MAX_MEMORY_SIZE= 1000*1024;
std::string CONFIG_FILENAME1 = "/home/vadimir_kluchinskiy/Desktop/CourseExamples/KluchinskiyCourseProject/etc/config.txt";
std::string OUTPUT_FILENAME1 = "/home/vadimir_kluchinskiy/Desktop/CourseExamples/KluchinskiyCourseProject/etc/scan_result.txt";
std::vector<std::string> RootDirectories;
char** Dirs;
Notify* NTF;
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
        std::ofstream file(OUTPUT_FILENAME1, std::ios_base::app);
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
void set_dirs(){
    size_t size = RootDirectories.size();
    Dirs = new char*[size + 1]; // +1 для указателя на null в конце

    for (size_t i = 0; i < size; ++i) {
        Dirs[i] = new char[RootDirectories[i].size() + 1]; // +1 для нулевого символа в конце строки
        strcpy(Dirs[i], RootDirectories[i].c_str());
    }
    Dirs[size]=NULL;
}
void clear_dirs(){
    size_t size = RootDirectories.size();
    if(size) {
        for (size_t i = 0; i < size; ++i) {
            delete[] Dirs[i];
        }
        delete[] Dirs;
    }
}
void set_directories(){
    std::string directory;
    std::ifstream config_file(CONFIG_FILENAME1);

    if (config_file.is_open()) {
        while (getline(config_file, directory)) {
            RootDirectories.push_back(directory);
        }
        config_file.close();
    } else {
        syslog(LOG_INFO, "ERROR WHILE READING CONFIG FILE");
    }
    set_dirs();
}
void handle_sigint(int sig) {
    printf("SIGINT SIGNAL...\n");
    syslog(LOG_INFO, "FINISH PROCESS BY SIGINT SIGNAL...\n");
    clear_dirs();
    freeNotify(NTF);
    syslog(LOG_INFO, "PROCESS WAS FINISHED BY SIGINT SIGNAL...\n");
    exit(EXIT_SUCCESS);

}
int main(int argc, char *argv[]){

    pid_t pid;
    int x_fd;
    //Step1
    //Fork off the parent process
    pid = fork();
    //If error occured
    if(pid<0){
        exit(EXIT_FAILURE);
    }
    //Success: Let the parent terminate
    if(pid>0){
        printf("Parent pid = %d DaemonPid = %d \n",getpid(),pid);
        exit(EXIT_SUCCESS);
    }
    //STEP2
    //On success: The child process becomes session leader and process group leader
    if(setsid()<0){
        exit(EXIT_FAILURE);
    }
    //STEP3
    //Catch, ignore and handle signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, handle_sigint);

    //STEP5
    //Set the new file permissions that is created by daemon
    umask(777);
    /*Change the working directory to the root directory
     * if the current directory is on some mounted file system
     * so daemon process will not let the mounted file system to unmount
     */
    //STEP6
    if ((chdir("/")) < 0) {
        printf("Error changing directory!\n");
        exit(1);
    }
    //Close all file descriptors
    //STEP 7
    for(x_fd = sysconf(_SC_OPEN_MAX);x_fd>=0;--x_fd){
        close(x_fd);
    }
    //STEP8
    //Logging Errors/Info in the syslog system
    if(argc==2) {
        CONFIG_FILENAME1 = argv[1];
    }
    set_directories();
    long page_size = sysconf(_SC_PAGESIZE);
    size_t path_to_statm_size = 64;
    char path_to_statm[path_to_statm_size];
    snprintf(path_to_statm, path_to_statm_size, "/proc/%d/statm", getpid());
    const long max_memory_pages = MAX_MEMORY_SIZE / page_size;

    //char* dirs[2] = {argv[1], NULL};
    printf("Start to watch %s page_size=%ld max_memory_pages=%ld\n", Dirs[0], page_size, max_memory_pages);

    uint32_t mask = IN_ALL_EVENTS;
    //uint32_t mask = IN_MODIFY | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;

    NTF = initNotify(Dirs, mask, "^\\.");
    if (NTF == NULL)
    {
        exit(EXIT_FAILURE);
    }
    std::string prev_path = "0";
    uint32_t prev_mask = 0;

    while(true)
    {
        char* path = NULL;
        uint32_t mask = 0;
        uint32_t cookie = 0;
        if (-1 == waitNotify(NTF, &path, &mask, 0, &cookie))
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
           // printf("path=NULL\n");
            exit(EXIT_FAILURE);
        }
        if (mask & IN_ACCESS)
        {
           // std::string action_name ="File was accessed.";
           // int exit_code = insert_action(path,action_name,cookie);
            //printf("File was accessed. \t%s cookie=%d\n", path, cookie);
          //  if(exit_code){
           //     break;
            //}
        }
        if (mask & IN_MODIFY)
        {
            std::string action_name ="File was modified.";
            int exit_code = insert_action(path,action_name,cookie);
          //  printf("File was modified. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_ATTRIB)
        {
            std::string action_name ="Metadata changed.";
            int exit_code = insert_action(path,action_name,cookie);
           // printf("Metadata changed. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_CLOSE_WRITE)
        {
            std::string action_name ="Writtable file was closed.";
            int exit_code = insert_action(path,action_name,cookie);
            //printf("Writtable file was closed. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_CLOSE_NOWRITE)
        {
            std::string action_name ="Unwrittable file closed.";
            int exit_code = insert_action(path,action_name,cookie);
           // printf("Unwrittable file closed.\t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_OPEN)
        {
            std::string action_name ="File was opened.";
            int exit_code = insert_action(path,action_name,cookie);
           // printf("File was opened.  \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_MOVED_FROM)
        {
            std::string action_name ="File was moved from "+std::string(path);
            int exit_code = insert_action(path,action_name,cookie,true);
            //printf("File was moved from \t%s. cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_MOVED_TO)
        {
            std::string action_name ="File was moved to "+std::string(path);
            int exit_code = insert_action(path,action_name,cookie);
           // printf("File was moved to \t%s. cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_CREATE)
        {
            std::string action_name ="Subfile was created.";
            int exit_code = insert_action(path,action_name,cookie);
           // printf("Subfile was created. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_DELETE)
        {
            std::string action_name ="Subfile was deleted.";
            int exit_code = insert_action(path,action_name,cookie);
           // printf("Subfile was deleted. \t%s cookie=%d\n", path, cookie);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_DELETE_SELF)
        {
            std::string action_name ="Self was deleted.";
            int exit_code = insert_action(path,action_name,cookie);
           // printf("Self was deleted.%s\n", path);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_MOVE_SELF)
        {
            std::string action_name = "Self was moved.";
            int exit_code = insert_action(path,action_name,cookie);
            //printf("Self was moved. %s\n", path);
            if(exit_code){
                break;
            }
        }
        if (mask & IN_Q_OVERFLOW)
        {
            syslog(LOG_INFO, "EVENT QUEUED OVERFLOW\n");
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

    clear_dirs();
    freeNotify(NTF);

    return 0;
}
#endif