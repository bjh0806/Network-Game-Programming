#include "..\..\Common.h"

int main(int argc, char *argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
		return 1;
	
	printf("wVersion: %d.%d\n", wsa.wVersion & 0x00ff, (wsa.wVersion >> 8) & 0x00ff);
	printf("wHighVersion: %d.%d\n", wsa.wHighVersion & 0x00ff, (wsa.wHighVersion >> 8) & 0x00ff);
	printf("szDescription: %s\n", wsa.szDescription);
	printf("szSystemStatus: %s\n", wsa.szSystemStatus);

	// ���� ����
	WSACleanup();
	return 0;
}