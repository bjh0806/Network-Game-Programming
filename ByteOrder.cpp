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
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("IsLittleEndian()의 리턴값: %s\n", IsLittleEndian() ? "true" : "false");
	printf("IsBigEndian()의 리턴값: %s\n", IsBigEndian() ? "true" : "false");

	// 윈속 종료
	WSACleanup();
	return 0;
}