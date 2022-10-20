#include "..\..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    1024

CONSOLE_SCREEN_BUFFER_INFO NowCur;
int count;

void gotoxy(int x, int y)
{
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
    int retval;
    SOCKET client_sock = (SOCKET)arg;
    struct sockaddr_in clientaddr;
    char addr[INET_ADDRSTRLEN];
    int addrlen;
    char buf[BUFSIZE + 1];

    // 클라이언트 정보 얻기
    addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    CONSOLE_SCREEN_BUFFER_INFO Cur = NowCur;
    
    while (1) {
        char file_name[BUFSIZE];
        memset(&file_name, 0, sizeof(file_name));
        retval = recv(client_sock, file_name, sizeof(file_name), 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }

        long long file_size = 0;
        retval = recv(client_sock, (char*)&file_size, sizeof(file_size), 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }

        FILE* file = fopen(file_name, "wb");
        long long size = 0;
        long long last = 0;

        while (1) {
            retval = recv(client_sock, buf, BUFSIZE, 0);

            if (retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }

            else if (size == file_size) {
                break;
            }

            else {
                fwrite(buf, 1, retval, file);

                size += retval;

                if (last != (int)((float)size / file_size * 100)) {
                    gotoxy(Cur.dwCursorPosition.X, Cur.dwCursorPosition.Y);
                    printf("전송 받을 파일명: %s %d%% 전송...", file_name, (int)((float)size / file_size * 100));
                }

                last = (int)((float)size / file_size * 100);
            }
        }

        // 소켓 닫기
        printf("\n파일 전송을 완료했습니다.\n");
        fclose(file);
        break;
    }

    // 소켓 닫기
    closesocket(client_sock);
    printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n\n\n",
        addr, ntohs(clientaddr.sin_port));
    count--;
    return 0;
}

int main(int argc, char* argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    HANDLE hThread;

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        count++;

        // 접속한 클라이언트 정보 출력
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        if (count != 1) {
           printf("\n\n\n\n");
        }
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            addr, ntohs(clientaddr.sin_port));
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &NowCur);

        // 스레드 생성
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    // 소켓 닫기
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}