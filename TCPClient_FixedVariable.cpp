#include "..\..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    1024

int main(int argc, char* argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ����
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

    // ������ ��ſ� ����� ����
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
    printf("������ ���� �̸��� �Է��ϼ���: ");
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
            printf("���� ������ �Ϸ��߽��ϴ�.\n");
            break;
        }
    }

    fclose(file);

    //const char *testdata[] = {
    //   "�ȳ��ϼ���",
    //   "�ݰ�����",
    //   "���õ��� �� �̾߱Ⱑ ���� �� ���׿�",
    //   "���� �׷��׿�",
    //};
    //int len;

    //// ������ ������ ���
    //for (int i = 0; i < 4; i++) {
    //   // ������ �Է�(�ùķ��̼�)
    //   len = (int)strlen(testdata[i]);
    //   strncpy(buf, testdata[i], len);

    //   // ������ ������(���� ����)
    //   retval = send(sock, (char *)&len, sizeof(int), 0);
    //   if (retval == SOCKET_ERROR) {
    //      err_display("send()");
    //      break;
    //   }

    //   // ������ ������(���� ����)
    //   retval = send(sock, buf, len, 0);
    //   if (retval == SOCKET_ERROR) {
    //      err_display("send()");
    //      break;
    //   }
    //   printf("[TCP Ŭ���̾�Ʈ] %d+%d����Ʈ�� "
    //      "���½��ϴ�.\n", (int)sizeof(int), retval);
    //}

    // ���� �ݱ�
    closesocket(sock);

    // ���� ����
    WSACleanup();
    return 0;
}