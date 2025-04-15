#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib") // ��Ŀ�� ���̺귯�� ������� �����ϴ� ��
#define PACKET_SIZE 1024

SOCKET server_socket;

// recv() return ���� ���� byte �� or �������� 0 or ������ ���� -1
// recv()�� ���ۿ� �޽����� �޾ƿ��� '\0'�� �߰����� ����
void proc_recv() {
	char buffer[PACKET_SIZE];
	
	while (true) {
		
		int msg_cnt = recv(server_socket, buffer, PACKET_SIZE, 0);
		if (msg_cnt == -1) {
			cout << " �޽��� recv() ���� " << endl;
			break;
		}
		else if (msg_cnt == 0) {
			cout << " ������ ����Ǿ����ϴ�." << endl;
			break;
		}
		else {
			buffer[msg_cnt] = '\0';
			cout << "���� �޽���: " << buffer << endl;
		}
	}

	//while (!WSAGetLastError()) {
	//	ZeroMemory(&buffer, PACKET_SIZE);
	//	recv(server_socket, buffer, PACKET_SIZE, 0);
	//	cout << "���� �޽���: " << buffer << endl;
	//}
}

/*
	Ŭ���̾�Ʈ
	1. Winsock Init() �ʿ�
	2. Socket create()
	3. connect()
	4. send(), recv() ����
	5. close()
*/

int main(void) {
	
	WSADATA wsa; // ���̺귯�� �ʱ�ȭ�� ����
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
		cout << " WSA STARTUP FAILED!! " << endl;
		return 1;
	}
	// ���� ���� ����
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// ������ �ּҿ� IP�� �������ִ� ����
	SOCKADDR_IN addr = { };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4444);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ���� ������ IP�� �Է��ϴµ� ����� ���� IP�� �������

	while (1) {
		if (!connect(server_socket, (SOCKADDR*)&addr, sizeof(addr))) break;
	}
	cout << " Client�� Server�� ����Ǿ����ϴ�. \n";

	thread proc1(proc_recv);

	char msg[PACKET_SIZE] = { 0 };
	while (!WSAGetLastError()) {
		ZeroMemory(&msg, PACKET_SIZE);
		cin.getline(msg,PACKET_SIZE);
		send(server_socket, msg, strlen(msg), 0); // ���� send�� �����ϸ� ����
	}
	proc1.join();

	closesocket(server_socket);

	WSACleanup();
	return 0;

}