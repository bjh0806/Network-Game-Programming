#include "..\..\Common.h"

char *SERVERIP = (char *)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE    1024

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ������ ��ſ� ����� ����
	char file_name[256];
	printf("������ ���� �̸��� �Է��ϼ���: ");
	scanf("%s", &file_name);
	retval = send(sock, file_name, 256, 0);

	FILE* file = fopen(file_name, "rb");

	fseek(file, 0, SEEK_END);
	long long file_size = ftell(file);
	retval = send(sock, (char*)&file_size, sizeof(file_size), 0);

	char buf[BUFSIZE];
	long long num = 0;
	long long size = 0;
	rewind(file);

	while (1) {
		size = fread(buf, 1, BUFSIZE, file);

		if (size > 0) {
			retval = send(sock, buf, size, 0);

			if (retval == SOCKET_ERROR) {
				err_display("send()");
				break;
			}

			num += size;
		}

		else if (size == 0 && num == file_size) {
			printf("���� ������ �Ϸ��߽��ϴ�.\n");
			break;
		}
	}

	fclose(file);

	// ���� �ݱ�
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}
