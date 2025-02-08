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
		logError("WSAStartup �ʱ�ȭ ����");
		logError(to_string(WSAGetLastError()));
		return -1;
	}

	// ���� ����
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket == INVALID_SOCKET) {
		logError("���� ���� ����");
		
		WSACleanup();
		return -1;
	}

	// ���ε�
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; // ��� IP���� ���� ���
	serverAddr.sin_port = htons(12345);
	

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		logError("���ε� ����");
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	sockaddr_in boundAddr;
	int addrLen = sizeof(boundAddr);

	if (getsockname(serverSocket, (sockaddr*)&boundAddr, &addrLen) == 0) {
		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &boundAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
		logInfo("������ " + string(ipStr) + ":" + to_string(ntohs(boundAddr.sin_port)) + "���� �������Դϴ�.");
	}
	else {
		logError("���� IP Ȯ�� ����");
	}
	
	if (listen(serverSocket, backlog) == SOCKET_ERROR) {
		logError("���� ��� ����");
		
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	logInfo("Ŭ���̾�Ʈ ���� ��� ��...");

	clientSocket = accept(serverSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		logError("���� ���� ����");
		
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	logInfo("Ŭ���̾�Ʈ ���� ����");

	int receivedBytes = recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
	if (receivedBytes == SOCKET_ERROR) {
		logError("�����͸� ���� �� �����ϴ�.");
		logError(to_string(WSAGetLastError()));
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}
	else if (receivedBytes == 0) {
		logInfo("Ŭ���̾�Ʈ ���� �����");
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}

	string receivedData(buffer.begin(), buffer.begin() + receivedBytes);
	logInfo("���� ������" + receivedData);

	if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
		logError("�����͸� ���� �� �����ϴ�.");
		
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	logInfo("�޽��� ���� ����");

	
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}

