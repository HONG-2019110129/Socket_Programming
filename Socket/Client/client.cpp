#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib") // 링커에 라이브러리 넣으라고 지시하는 것
#define PACKET_SIZE 1024

SOCKET server_socket;

// recv() return 값은 받은 byte 수 or 정상종료 0 or 비정상 오류 -1
// recv()는 버퍼에 메시지만 받아오지 '\0'을 추가하지 않음
void proc_recv() {
	char buffer[PACKET_SIZE];
	
	while (true) {
		
		int msg_cnt = recv(server_socket, buffer, PACKET_SIZE, 0);
		if (msg_cnt == -1) {
			cout << " 메시지 recv() 실패 " << endl;
			break;
		}
		else if (msg_cnt == 0) {
			cout << " 연결이 종료되었습니다." << endl;
			break;
		}
		else {
			buffer[msg_cnt] = '\0';
			cout << "받은 메시지: " << buffer << endl;
		}
	}

	//while (!WSAGetLastError()) {
	//	ZeroMemory(&buffer, PACKET_SIZE);
	//	recv(server_socket, buffer, PACKET_SIZE, 0);
	//	cout << "받은 메시지: " << buffer << endl;
	//}
}

/*
	클라이언트
	1. Winsock Init() 필요
	2. Socket create()
	3. connect()
	4. send(), recv() 실행
	5. close()
*/

int main(void) {
	
	WSADATA wsa; // 라이브러리 초기화를 위함
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
		cout << " WSA STARTUP FAILED!! " << endl;
		return 1;
	}
	// 서버 소켓 생성
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// 서버의 주소와 IP를 설정해주는 과정
	SOCKADDR_IN addr = { };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4444);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 여기 서버의 IP를 입력하는데 현재는 같은 IP라 상관없음

	while (1) {
		if (!connect(server_socket, (SOCKADDR*)&addr, sizeof(addr))) break;
	}
	cout << " Client가 Server에 연결되었습니다. \n";

	thread proc1(proc_recv);

	char msg[PACKET_SIZE] = { 0 };
	while (!WSAGetLastError()) {
		ZeroMemory(&msg, PACKET_SIZE);
		cin.getline(msg,PACKET_SIZE);
		send(server_socket, msg, strlen(msg), 0); // 만약 send를 실패하면 종료
	}
	proc1.join();

	closesocket(server_socket);

	WSACleanup();
	return 0;

}