#include "..\..\Common.h"

int main(int argc, char *argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	for (int i = 1; i < argc; ++i) {
		struct hostent* host = gethostbyname(argv[i]);

		printf("�Է¹��� ������ �̸�: %s\n", argv[i]);

		printf("- ȣ��Ʈ ���� -\n");

		for (int j = 0; host->h_aliases[j] != NULL; ++j) {
			printf("%s\n", host->h_aliases[j]);
		}

		printf("- IPv4 �ּ� -\n");

		for (int j = 0; host->h_addr_list[j] != NULL; ++j) {
			char str[INET_ADDRSTRLEN];

			printf("%s\n", inet_ntop(AF_INET, host->h_addr_list[j], str, sizeof(str)));
		}

		printf("--------------------\n");
	}

	// ���� ����
	WSACleanup();
	return 0;
}
