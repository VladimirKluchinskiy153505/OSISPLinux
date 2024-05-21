#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
int Connection;
std::string Ip;
unsigned short int PortToNetwork;
std::string Username;

void ClientHandler() {
    int msg_size;
    while (true) {
        recv(Connection, (char*)&msg_size, sizeof(int), 0);
        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        recv(Connection, msg, msg_size, 0);
        std::cout << msg << '\n';
        delete[] msg;
    }
}
int main(int argc, char* argv[]) {
    if (argc > 3) {
        Ip = argv[1];
        PortToNetwork = std::strtoul(argv[2], nullptr, 10);
        Username = argv[3];
    } else {
        Ip = "127.0.0.1";
        PortToNetwork = 1111;
        Username = "User";
    }
    std::cout << "Great Chat\n" << "Username: " << Username << "\nIp : " << Ip << " PortToNetwork: " << PortToNetwork << '\n';

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PortToNetwork);
    serverAddr.sin_addr.s_addr = inet_addr(Ip.c_str());
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    Connection = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(Connection, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        std::cout << "Error, failed connect to server\n";
        exit(1);
    }
    std::cout << "Connected!\n";

    std::thread(ClientHandler).detach();

    std::string msg1;
    while (true) {
        std::getline(std::cin, msg1);
        msg1 = Username + ": " + msg1;
        int msg_size = msg1.size();
        send(Connection, (char*)&msg_size, sizeof(int), 0);
        send(Connection, msg1.c_str(), msg_size, 0);
        usleep(10000); // Подождем 10 миллисекунд
    }

    return 0;
}