#include "..\..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    1024

int main(int argc, char* argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    char* SERVERIP = (char*)argv[1];

    // connect()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    // 데이터 통신에 사용할 변수
    FILE* file = fopen(argv[2], "rb");
    if (file == NULL) {
        err_display("fopen()");
    }

    char file_name[BUFSIZE];
    memset(&file_name, 0, sizeof(file_name));
    sprintf(file_name, argv[2]);
    retval = send(sock, file_name, sizeof(file_name), 0);

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    retval = send(sock, (char*)&file_size, sizeof(file_size), 0);

    /*char file_name[256];
    printf("전송할 파일 이름을 입력하세요: ");
    scanf("%s", &file_name);
    retval = send(sock, file_name, 256, 0);

    FILE* file = fopen(file_name, "rb");

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    retval = send(sock, (char*)&file_size, sizeof(file_size), 0);*/

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
            printf("파일 전송을 완료했습니다.\n");
            break;
        }
    }

    fclose(file);

    //const char *testdata[] = {
    //   "안녕하세요",
    //   "반가워요",
    //   "오늘따라 할 이야기가 많을 것 같네요",
    //   "저도 그렇네요",
    //};
    //int len;

    //// 서버와 데이터 통신
    //for (int i = 0; i < 4; i++) {
    //   // 데이터 입력(시뮬레이션)
    //   len = (int)strlen(testdata[i]);
    //   strncpy(buf, testdata[i], len);

    //   // 데이터 보내기(고정 길이)
    //   retval = send(sock, (char *)&len, sizeof(int), 0);
    //   if (retval == SOCKET_ERROR) {
    //      err_display("send()");
    //      break;
    //   }

    //   // 데이터 보내기(가변 길이)
    //   retval = send(sock, buf, len, 0);
    //   if (retval == SOCKET_ERROR) {
    //      err_display("send()");
    //      break;
    //   }
    //   printf("[TCP 클라이언트] %d+%d바이트를 "
    //      "보냈습니다.\n", (int)sizeof(int), retval);
    //}

    // 소켓 닫기
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}