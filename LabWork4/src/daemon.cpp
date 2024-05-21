#include<stdio.h>
#include<unistd.h> //fork
#include<stdlib.h> //for exit
#include<string.h>
#include<sys/stat.h> //for unmask
#include <sys/types.h>
#include<syslog.h>
#include <signal.h>
#define LOGGING "SCAN UPDATED!! = %d\n"
#include "../headers/file_scan_lib.h"
std::string CONFIG_FILENAME1 = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/LabWork4/etc/config.txt";
std::string OUTPUT_FILENAME1 = "/home/vadimir_kluchinskiy/Desktop/ClionProjects/LabWork4/etc/scan_file_result.txt";
std::vector<std::string> RootDirectories;
void set_directories(){
    RootDirectories.clear();
    std::string directory;
    std::ifstream config_file(CONFIG_FILENAME1);

    if (config_file.is_open()) {
        while (getline(config_file, directory)) {
            RootDirectories.push_back(directory);
        }
        config_file.close();
    } else {
        std::cerr << "Unable to open config";
    }
}
void handle_sigint(int sig) {
    printf("SIGINT SIGNAL...\n");
    syslog(LOG_INFO, "FINISH PROCESS BY SIGINT SIGNAL...");
    exit(EXIT_SUCCESS);
}
void handle_sighub(int sig){
    set_directories();
    syslog(LOG_INFO, "RECONFIGURING BY BY SIGHUP SIGNAL...");
}
int main(){
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
    signal(SIGHUP, handle_sighub);
    signal(SIGINT, handle_sigint);
    //STEP4
    //Fork off for the second time
//    pid = fork();
//    //An error occured
//
//    if(pid<0){
//        exit(EXIT_FAILURE);
//    }
//    //Success: Let the parameter terminate
//    if(pid>0){
//        printf("Parent pid = %d DaemonPid = %d \n",getpid(),pid);
//        exit(EXIT_SUCCESS);
//    }
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
    set_directories();
    int count=0;
    openlog("Logs", LOG_PID, LOG_USER);
    while(true){
        sleep(30);
        scan_files(RootDirectories,OUTPUT_FILENAME1);
        syslog(LOG_INFO, LOGGING, ++count);
    }
    closelog();
    return 1;
}