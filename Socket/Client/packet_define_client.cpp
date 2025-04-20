#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib") // ��Ŀ�� ���̺귯�� ������� �����ϴ� ��
#define PACKET_SIZE 1024

/*
struct PacketHeader {
	uint8_t type;   // ��Ŷ Ÿ��
	uint8_t sender; // �۽���
	uint16_t size;  // ���� payload ũ��
};*/

enum class ModuleType : uint8_t {
	Control_center = 0,
	Missile_launcher = 1,
	Radar =2,
	Tracker = 3
};

typedef struct Packet {
	ModuleType sender;
	float x, y, z;
	float velocity;
}Packet;

SOCKET client_socket;

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
		else {
			cout << "msg ���ۼ��� !!" << endl;
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
	// Ŭ���̾�Ʈ ���� ����
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// ������ �ּҿ� IP�� �������ִ� ����
	SOCKADDR_IN addr = { };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7777);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ���� ������ IP�� �Է��ϴµ� ����� ���� IP�� �������

	while (1) {
		if (!connect(client_socket, (SOCKADDR*)&addr, sizeof(addr))) break;
	}
	cout << " Client�� Server�� ����Ǿ����ϴ�. \n";

	thread proc2(proc_send);
	thread proc1(proc_recv);
	proc2.join();
	proc1.join();

	closesocket(client_socket);

	WSACleanup();
	return 0;

}