#define _WINSOCK_DEPRECATED_NO_WARNINGS // winsock c4996 처리

#include <iostream>
#include <winsock2.h>
#include <thread>
using namespace std;

#pragma comment(lib,"ws2_32.lib") // ws2_32.lib 라이브러리를 링크
#define PACKET_SIZE 1024 // 송수신 버퍼 사이즈 1024로 설정
SOCKET server_socket, client_socket;

void proc_recvs() {
	char buffer[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError()) {
		ZeroMemory(&buffer, PACKET_SIZE); // 버퍼 초기화
		recv(client_socket, buffer, PACKET_SIZE, 0);
		cout << "받은 메세지: " << buffer << endl;
	}
}

int main() {
	WSADATA wsa; // Windows 소켓 구현에 대한 구조체 생성

	// WSAStartup을 통해 wsadata 초기화
	// MAKEWORD(2,2) 매개 변수는 시스템에서 Winsock 버전 2.2를 요청
	// WSAStartup은 성공시 0, 실패시 SOCKET_ERROR를 리턴하므로
	// 리턴값이 0인지 검사하여 에러 여부 확인
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "WSAStartup failed" << endl;
		return 1;
	}

	// 서버가 클라이언트 연결을 수신 대기할 수 있도록 소켓 생성
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 주소 패밀리, IP 주소 및 포트 번호에 대한 정보를 보유할 sockaddr 구조체 생성
	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET; // IPv4 기반의 TCP, UDP 프로토콜 사용
	addr.sin_port = htons(4444);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind 함수를 통해 생성된 소켓 및 sockaddr 구조를 매개 변수로 전달
	bind(server_socket, (SOCKADDR*)&addr, sizeof(addr));
	listen(server_socket, SOMAXCONN); // 소켓에서 수신 대기
	// SOMAXCONN은 Winsock 공급자에게 큐에 있는 최대 적정 수의 보류 중인 연결을 허용하도록 지시하는 특수 상수

	SOCKADDR_IN client = {};
	int client_size = sizeof(client);
	//ZeroMemory(&client, client_size);

	// 클라이언트에서 연결을 수락하기 위해 ClientSocket이라는 임시 소켓 개체 생성
	client_socket = accept(server_socket, (SOCKADDR*)&client, &client_size);

	// 연결 시 클라이언트 정보 출력
	if (!WSAGetLastError()) {
		std::cout << "연결 완료" << endl;
		std::cout << "Client IP: " << inet_ntoa(client.sin_addr) << endl;
		std::cout << "Port: " << ntohs(client.sin_port) << endl;
	}

	// client로부터 메세지를 수신하는 함수를 thread에 등록
	thread proc(proc_recvs);

	char msg[PACKET_SIZE] = { 0 }; // 메세지 입력받을 버퍼 생성
	while (!WSAGetLastError()) {
		ZeroMemory(&msg, PACKET_SIZE); // 버퍼 초기화하고, 입력받은 메세지를 클라이언트에게 send
		cin.getline(msg, PACKET_SIZE);
		send(client_socket, msg, strlen(msg), 0);
	}

	proc.join(); // 실행중인 thread의 작업 완료될때까지 대기

	closesocket(client_socket); // client, server socket 닫기
	closesocket(server_socket);

	WSACleanup(); // 리소스 해제
	return 0;
}