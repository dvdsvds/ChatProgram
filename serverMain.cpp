#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void logError(const string& errMsg) {
    cout << "[ERROR] " << errMsg << endl;
}

void logInfo(const string& infoMsg) {
    cout << "[INFO] " << infoMsg << endl;
}

int main() {
    const int BUFFER_SIZE = 1024;
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(serverAddr);
    int backlog = 5;

    vector<char> buffer(BUFFER_SIZE);
    string message = "Hello";

    // ¿¿ ¿¿ ¿¿
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        logError("¿¿ ¿¿ ¿¿ ¿¿");
        return -1;
    }

    // ¿¿ ¿¿ ¿¿
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // ¿¿ IP ¿¿¿ ¿¿
    serverAddr.sin_port = htons(12345);

    // ¿¿¿
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        logError("¿¿¿ ¿¿");
        close(serverSocket);
        return -1;
    }

    // ¿¿ ¿¿ ¿¿
    char ipStr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &serverAddr.sin_addr, ipStr, INET_ADDRSTRLEN) == nullptr) {
        logError("IP ¿¿ ¿¿");
    } else {
        logInfo("¿¿¿ " + string(ipStr) + ":" + to_string(ntohs(serverAddr.sin_port)) + "¿¿ ¿¿ ¿...");
    }

    // ¿¿¿
    if (listen(serverSocket, backlog) == -1) {
        logError("¿¿ ¿¿");
        close(serverSocket);
        return -1;
    }

    logInfo("¿¿¿¿¿ ¿¿ ¿¿ ¿...");

    // ¿¿¿¿¿ ¿¿ ¿¿
    clientSocket = accept(serverSocket, (struct sockaddr*)&serverAddr, &addrLen);
    if (clientSocket == -1) {
        logError("¿¿¿¿¿ ¿¿ ¿¿");
        close(serverSocket);
        return -1;
    }

    logInfo("¿¿¿¿¿ ¿¿ ¿¿");

    // ¿¿¿ ¿¿
    int receivedBytes = recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
    if (receivedBytes == -1) {
        logError("¿¿¿ ¿¿ ¿¿");
        close(clientSocket);
        close(serverSocket);
        return -1;
    } else if (receivedBytes == 0) {
        logInfo("¿¿¿¿¿¿ ¿¿¿ ¿¿¿¿¿¿.");
        close(clientSocket);
        close(serverSocket);
        return 0;
    }

    string receivedData(buffer.begin(), buffer.begin() + receivedBytes);
    logInfo("¿¿¿ ¿¿¿: " + receivedData);

    // ¿¿¿¿¿¿ ¿¿¿ ¿¿
    if (send(clientSocket, message.c_str(), message.length(), 0) == -1) {
        logError("¿¿¿ ¿¿ ¿¿");
        close(clientSocket);
        close(serverSocket);
        return -1;
    }

    logInfo("¿¿¿ ¿¿ ¿¿");

    // ¿¿ ¿¿
    close(clientSocket);
    close(serverSocket);

    return 0;
}

