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

using namespace std;

#define PORT 12345
#define MAX_CLIENTS 10

void logError(const string& errMsg) {
    cout << "[ERROR] " << errMsg << endl;
}

void logInfo(const string& infoMsg) {
    cout << "[INFO] " << infoMsg << endl;
}

void handleClient(int clientSocket) {
    const int BUFFER_SIZE = 1024;
    vector<char> buffer(BUFFER_SIZE);
    int receivedBytes;

    while(true) {
        receivedBytes = recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
        if (receivedBytes <= 0) {
            logInfo("클라이언트가 연결을 종료했습니다.");
            break;
        }

        string receivedData(buffer.begin(), buffer.begin() + receivedBytes);
        cout << "클라이언트 : " + receivedData << endl;
    }
}
int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(serverAddr);
    int backlog = 5;

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
    if (listen(serverSocket, backlog) == -1) {
        logError("연결 대기 수락 실패");
        logError(strerror(errno));
        close(serverSocket);
        return -1;
    }

    logInfo("클라이언트 연결 대기 중...");

    // 클라이언트 연결 대기
    clientSocket = accept(serverSocket, (struct sockaddr*)&serverAddr, &addrLen);
    if (clientSocket == -1) {
        logError("클라이언트 연결 실패");
        logError(strerror(errno));
        close(serverSocket);
        return -1;
    }

    logInfo("클라이언트 연결 완료");

    thread clientThread(handleClient, clientSocket);
    clientThread.detach();

    // 소켓 종료
    close(serverSocket);

    return 0;
}

