#include "..\..\Common.h"

bool IsLittleEndian()
{
	u_short x = 0x1234;

	if (htons(x) != x) {
		return true;
	}

	else {
		return false;
	}
}

bool IsBigEndian()
{
	u_short x = 0x1234;

	if (htons(x) == x) {
		return true;
	}

	else {
		return false;
	}
}

int main(int argc, char *argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("IsLittleEndian()�� ���ϰ�: %s\n", IsLittleEndian() ? "true" : "false");
	printf("IsBigEndian()�� ���ϰ�: %s\n", IsBigEndian() ? "true" : "false");

	// ���� ����
	WSACleanup();
	return 0;
}