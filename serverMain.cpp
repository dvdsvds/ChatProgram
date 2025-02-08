#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>

#pragma comment(lib, "ws2_32")

using namespace std;

void logError(const string& errMsg) {
	cout << "[ERROR] " << errMsg << endl;;
}
void logInfo(const string& infoMsg) {
	cout << "[INFO] " << infoMsg << endl;;
}
int main() {
	const int BUFFER_SIZE = 1024;
	SOCKET serverSocket, clientSocket;
	sockaddr_in serverAddr;
	int backlog = 5;

	vector<char> buffer(BUFFER_SIZE);
	string message = "Hello";
	WSAData wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		logError("WSAStartup 초기화 실패");
		logError(to_string(WSAGetLastError()));
		return -1;
	}

	// 소켓 생성
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket == INVALID_SOCKET) {
		logError("소켓 생성 실패");
		
		WSACleanup();
		return -1;
	}

	// 바인드
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; // 모든 IP에서 접근 허용
	serverAddr.sin_port = htons(12345);
	

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		logError("바인드 실패");
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	sockaddr_in boundAddr;
	int addrLen = sizeof(boundAddr);

	if (getsockname(serverSocket, (sockaddr*)&boundAddr, &addrLen) == 0) {
		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &boundAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
		logInfo("서버가 " + string(ipStr) + ":" + to_string(ntohs(boundAddr.sin_port)) + "에서 실행중입니다.");
	}
	else {
		logError("서버 IP 확인 실패");
	}
	
	if (listen(serverSocket, backlog) == SOCKET_ERROR) {
		logError("연결 대기 실패");
		
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	logInfo("클라이언트 연결 대기 중...");

	clientSocket = accept(serverSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		logError("연결 수락 실패");
		
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	logInfo("클라이언트 연결 성공");

	int receivedBytes = recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
	if (receivedBytes == SOCKET_ERROR) {
		logError("데이터를 읽을 수 없습니다.");
		logError(to_string(WSAGetLastError()));
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}
	else if (receivedBytes == 0) {
		logInfo("클라이언트 연결 종료됨");
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}

	string receivedData(buffer.begin(), buffer.begin() + receivedBytes);
	logInfo("받은 데이터" + receivedData);

	if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
		logError("데이터를 보낼 수 없습니다.");
		
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	logInfo("메시지 전송 성공");

	
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}

