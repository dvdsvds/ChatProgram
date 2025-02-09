#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

#define PORT 12345
#define MAX_CLIENTS 10

map<int, string> clientNames;
vector<int> clientSockets;

void logError(const string& errMsg) {
    cout << "[ERROR] " << errMsg << endl;
}

void logInfo(const string& infoMsg) {
    cout << "[INFO] " << infoMsg << endl;
}

void broadcastMsg(const string& message, int senderSocket) {
    for(int clientSocket : clientSockets) {
        if(clientSocket != senderSocket) {
            send(clientSocket, message.c_str(), message.length(), 0);
        }
    }
}
void handleClient(int clientSocket) {
    const int BUFFER_SIZE = 1024;
    vector<char> buffer(BUFFER_SIZE);
    int receivedBytes;

    receivedBytes = recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
    string username(buffer.begin(), buffer.begin() + receivedBytes);
    if (receivedBytes > 0) {
        clientNames[clientSocket] = username;
        clientSockets.push_back(clientSocket);
        logInfo(username + "님이 입장하였습니다.");
        broadcastMsg(username + "님이 입장하였습니다.", clientSocket);
    }

    while(true) {
        receivedBytes = recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
        if (receivedBytes <= 0) {
            logInfo(username + "님이 연결을 종료했습니다.");
            clientNames.erase(clientSocket);
            clientSockets.erase(remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end());
            broadcastMsg(username + "님이 연결을 종료했습니다.", clientSocket);
            close(clientSocket);
            break;
        }

        string receivedData(buffer.begin(), buffer.begin() + receivedBytes);
        string message = clientNames[clientSocket] + " : " + receivedData;
        cout << message << endl;
        broadcastMsg(message, clientSocket);

    }
}
int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // 서버 소켓 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        logError("서버 소켓 생성 실패");
        logError(strerror(errno));
        return -1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 모든 IP 주소를 수락
    serverAddr.sin_port = htons(PORT);

    // 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        logError("바인딩 실패");
        logError(strerror(errno));
        close(serverSocket);
        return -1;
    }

    // 소켓 주소 확인
    char ipStr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &serverAddr.sin_addr, ipStr, INET_ADDRSTRLEN) == nullptr) {
        logError("IP 확인 실패");
        logError(strerror(errno));
    } else {
        logInfo("서버가 " + string(ipStr) + ":" + to_string(ntohs(serverAddr.sin_port)) + "에서 대기 중...");
    }

    // 연결 대기 수락
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        logError("연결 대기 수락 실패");
        logError(strerror(errno));
        close(serverSocket);
        return -1;
    }

    logInfo("클라이언트 연결 대기 중...");

    // 클라이언트 연결 대기
    while (true) {
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            logError("클라이언트 연결 실패");
            logError(strerror(errno));
            continue;
        }

        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    // 소켓 종료
    close(serverSocket);

    return 0;
}

