//
// Created by vadimir_kluchinskiy on 03.05.24.
//

#ifndef DAEMON_H
#define DAEMON_H
#include<string>
#include<vector>
#include<fstream>
#include<sys/stat.h> //for unmask
#include <signal.h>
#include<chrono>
#include <sys/syslog.h>
#include <iostream>
#include <sstream>
#include <ctime> // Для std::strftime
#include <iomanip>
#include <filesystem>
#include <pqxx/pqxx>
#include <unistd.h>
#include <cstring>
namespace fs = std::filesystem;
#endif DAEMON_H
