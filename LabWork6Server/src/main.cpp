#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

#define MAX_CONNECTIONS 100

int Connections[MAX_CONNECTIONS];
int Counter = 0;
std::string Ip;
unsigned short int PortToNetwork;

void Clienthandler(int index) {
    int msg_size;
    while (true) {
        int recvResult = recv(Connections[index], (char*)&msg_size, sizeof(int), 0);
        if (recvResult <= 0) {
            std::cout << "Client disconnected from thread " << index << std::endl;
            close(Connections[index]);
            Connections[index] = -1;
            return;
        }
        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        int recvResult1 = recv(Connections[index], msg, msg_size, 0);
        if (recvResult1 <= 0) {
            std::cout << "Client disconnected from thread " << index << std::endl;
            close(Connections[index]);
            Connections[index] = -1;
            return;
        }
        for (int i = 0; i < Counter; ++i) {
            if (i != index && Connections[i] >= 0) {
                send(Connections[i], (char*)&msg_size, sizeof(int), 0);
                send(Connections[i], msg, msg_size, 0);
            }
        }
        delete[] msg;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        Ip = argv[1];
        PortToNetwork = std::strtoul(argv[2], nullptr, 10);
    } else {
        Ip = "127.0.0.1";
        PortToNetwork = 1111;
    }

    int sListen = socket(AF_INET, SOCK_STREAM, 0);
    if (sListen == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PortToNetwork);
    serverAddr.sin_addr.s_addr = inet_addr(Ip.c_str());
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    if (bind(sListen, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    if (listen(sListen, SOMAXCONN) == -1) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int newConnection;
    while (true) {
        newConnection = accept(sListen, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (newConnection == -1) {
            std::cerr << "Accept failed" << std::endl;
        } else {
            std::cout << "Client is connected" << std::endl;
            std::string msg = "Hello. It's my first network program!\n";
            int msg_size = msg.size();
            send(newConnection, (char*)&msg_size, sizeof(int), 0);
            send(newConnection, msg.c_str(), msg_size, 0);
            Connections[Counter] = newConnection;
            ++Counter;
            std::thread(Clienthandler, Counter - 1).detach();
        }
    }
    close(sListen);
    return 0;
}