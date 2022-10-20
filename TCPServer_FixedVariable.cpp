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

    // Ŭ���̾�Ʈ ���� ���
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
                    printf("���� ���� ���ϸ�: %s %d%% ����...", file_name, (int)((float)size / file_size * 100));
                }

                last = (int)((float)size / file_size * 100);
            }
        }

        // ���� �ݱ�
        printf("\n���� ������ �Ϸ��߽��ϴ�.\n");
        fclose(file);
        break;
    }

    // ���� �ݱ�
    closesocket(client_sock);
    printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n\n\n",
        addr, ntohs(clientaddr.sin_port));
    count--;
    return 0;
}

int main(int argc, char* argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ����
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

    // ������ ��ſ� ����� ����
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

        // ������ Ŭ���̾�Ʈ ���� ���
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        if (count != 1) {
           printf("\n\n\n\n");
        }
        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
            addr, ntohs(clientaddr.sin_port));
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &NowCur);

        // ������ ����
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
    }

    // ���� �ݱ�
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}