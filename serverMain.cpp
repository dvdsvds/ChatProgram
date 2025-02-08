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

    // 서버 소켓 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        logError("서버 소켓 생성 실패");
        return -1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 모든 IP 주소를 수락
    serverAddr.sin_port = htons(12345);

    // 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        logError("바인딩 실패");
        close(serverSocket);
        return -1;
    }

    // 소켓 주소 확인
    char ipStr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &serverAddr.sin_addr, ipStr, INET_ADDRSTRLEN) == nullptr) {
        logError("IP 확인 실패");
    } else {
        logInfo("서버가 " + string(ipStr) + ":" + to_string(ntohs(serverAddr.sin_port)) + "에서 대기 중...");
    }

    // 연결 대기 수락
    if (listen(serverSocket, backlog) == -1) {
        logError("연결 대기 수락 실패");
        close(serverSocket);
        return -1;
    }

    logInfo("클라이언트 연결 대기 중...");

    // 클라이언트 연결 대기
    clientSocket = accept(serverSocket, (struct sockaddr*)&serverAddr, &addrLen);
    if (clientSocket == -1) {
        logError("클라이언트 연결 실패");
        close(serverSocket);
        return -1;
    }

    logInfo("클라이언트 연결 완료");

    // 데이터 수신
    int receivedBytes = recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
    if (receivedBytes == -1) {
        logError("데이터 수신 실패");
        close(clientSocket);
        close(serverSocket);
        return -1;
    } else if (receivedBytes == 0) {
        logInfo("클라이언트가 연결을 종료했습니다.");
        close(clientSocket);
        close(serverSocket);
        return 0;
    }

    string receivedData(buffer.begin(), buffer.begin() + receivedBytes);
    logInfo("수신된 데이터: " + receivedData);

    // 클라이언트로 데이터 전송
    if (send(clientSocket, message.c_str(), message.length(), 0) == -1) {
        logError("데이터 전송 실패");
        close(clientSocket);
        close(serverSocket);
        return -1;
    }

    logInfo("메시지 전송 완료");

    // 소켓 종료
    close(clientSocket);
    close(serverSocket);

    return 0;
}

