#define _WINSOCK_DEPRECATED_NO_WARNINGS // winsock c4996 ó��

#include <iostream>
#include <winsock2.h>
#include <thread>
#include <map>

using namespace std;

enum class ModuleType : uint8_t {
	Control_center = 0,
	Missile_launcher = 1,
	Radar = 2,
	Tracker = 3
};

typedef struct Packet {
	ModuleType sender;
	float x, y, z;
	float velocity;
}Packet;


#pragma comment(lib,"ws2_32.lib") // ws2_32.lib ���̺귯���� ��ũ
#define PACKET_SIZE 1024 // �ۼ��� ���� ������ 1024�� ����
SOCKET server_socket, client_socket;

void proc_send() {

	Packet msg = {};
	msg.sender = ModuleType::Control_center;
	while (true) {

		cout << " x,y,z ��ǥ: ";
		cin >> msg.x >> msg.y >> msg.z;
		cout << " velocity: ";
		cin >> msg.velocity;

		int len = send(client_socket, (char*)&msg, sizeof(msg), 0); // ���� send�� �����ϸ� ����
		if (len <= 0) {
			cerr << " msg send ����!! " << endl;
			break;
		}
	}
}

// recv() return ���� ���� byte �� or �������� 0 or ������ ���� -1
// recv()�� ���ۿ� �޽����� �޾ƿ��� '\0'�� �߰����� ����
void proc_recv() {
	char buffer[sizeof(Packet)];

	while (true) {

		int msg_cnt = recv(client_socket, buffer, sizeof(buffer), 0);
		if (msg_cnt == -1) {
			cout << " �޽��� recv() ���� " << endl;
			break;
		}
		else if (msg_cnt == 0) {
			cout << " ������ ����Ǿ����ϴ�." << endl;
			break;
		}
		else {
			Packet parsed;
			memcpy(&parsed, buffer, sizeof(Packet));
			cout << " ���ŵ� Packet ����\n";
			cout << "Sender: " << static_cast<int>(parsed.sender) << endl;
			cout << "X: " << parsed.x << ", Y: " << parsed.y << ", Z: " << parsed.z << endl;
			cout << "Velocity: " << parsed.velocity << endl;
		}
	}

}

int main() {
	WSADATA wsa; // Windows ���� ������ ���� ����ü ����

	// WSAStartup�� ���� wsadata �ʱ�ȭ
	// MAKEWORD(2,2) �Ű� ������ �ý��ۿ��� Winsock ���� 2.2�� ��û
	// WSAStartup�� ������ 0, ���н� SOCKET_ERROR�� �����ϹǷ�
	// ���ϰ��� 0���� �˻��Ͽ� ���� ���� Ȯ��
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "WSAStartup failed" << endl;
		return 1;
	}

	// ������ Ŭ���̾�Ʈ ������ ���� ����� �� �ֵ��� ���� ����
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// �ּ� �йи�, IP �ּ� �� ��Ʈ ��ȣ�� ���� ������ ������ sockaddr ����ü ����
	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET; // IPv4 ����� TCP, UDP �������� ���
	addr.sin_port = htons(7777);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind �Լ��� ���� ������ ���� �� sockaddr ������ �Ű� ������ ����
	bind(server_socket, (SOCKADDR*)&addr, sizeof(addr));
	listen(server_socket, SOMAXCONN); // ���Ͽ��� ���� ���
	// SOMAXCONN�� Winsock �����ڿ��� ť�� �ִ� �ִ� ���� ���� ���� ���� ������ ����ϵ��� �����ϴ� Ư�� ���

	SOCKADDR_IN client = {};
	int client_size = sizeof(client);
	//ZeroMemory(&client, client_size);

	// Ŭ���̾�Ʈ���� ������ �����ϱ� ���� ClientSocket�̶�� �ӽ� ���� ��ü ����
	client_socket = accept(server_socket, (SOCKADDR*)&client, &client_size);

	// ���� �� Ŭ���̾�Ʈ ���� ���
	if (!WSAGetLastError()) {
		std::cout << "���� �Ϸ�" << endl;
		std::cout << "Client IP: " << inet_ntoa(client.sin_addr) << endl;
		std::cout << "Port: " << ntohs(client.sin_port) << endl;
	}

	// client�κ��� �޼����� �����ϴ� �Լ��� thread�� ���
	thread proc1(proc_recv);
	thread proc2(proc_send);

	proc1.join(); // �������� thread�� �۾� �Ϸ�ɶ����� ���
	proc2.join();
	closesocket(client_socket); // client, server socket �ݱ�
	closesocket(server_socket);

	WSACleanup(); // ���ҽ� ����
	return 0;
}