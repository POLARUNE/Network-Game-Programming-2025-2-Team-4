#include "..\Common.h"
#include <direct.h>   // _getcwd(): 현재 작업 디렉터리 얻기

#define SERVERPORT 9000
#define BUFSIZE    512

DWORD WINAPI ClientThread(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;

    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);

    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
    printf("\n[TCP 서버] 클라이언트 접속 - IP 주소: %s, 포트 번호: %d\n",
        addr, ntohs(clientaddr.sin_port));

    // 파일 이름 길이 수신
    int nameLen;
    retval = recv(client_sock, (char*)&nameLen, sizeof(int), MSG_WAITALL);
    if (retval <= 0) { err_display("recv() - nameLen"); closesocket(client_sock); return 1; }

    // 파일 이름 수신
    char filename[MAX_PATH] = { 0 };
    retval = recv(client_sock, filename, nameLen, MSG_WAITALL);
    if (retval <= 0) { err_display("recv() - filename"); closesocket(client_sock); return 1; }
    filename[nameLen] = '\0';

    // 파일 크기 수신
    long long fileSize;
    retval = recv(client_sock, (char*)&fileSize, sizeof(long long), MSG_WAITALL);
    if (retval <= 0) { err_display("recv() - fileSize"); closesocket(client_sock); return 1; }

    printf("[서버] 파일 이름: %s\n", filename);
    printf("[서버] 파일 크기: %lld 바이트\n", fileSize);

    char cwd[MAX_PATH];
    _getcwd(cwd, sizeof(cwd));
    printf("[서버] 저장 위치: %s\n", cwd);

    FILE* fp = fopen(filename, "wb");
    if (!fp) { perror("fopen()"); closesocket(client_sock); return 1; }

    char buf[BUFSIZE];
    long long totalRecv = 0;
    int lastPercent = -1;

    while (totalRecv < fileSize)
    {
        int bytesToRead = (int)min(BUFSIZE, fileSize - totalRecv);
        retval = recv(client_sock, buf, bytesToRead, 0);
        if (retval <= 0) break;

        fwrite(buf, 1, retval, fp);
        totalRecv += retval;

        int percent = (int)((double)totalRecv / fileSize * 100);
        if (percent != lastPercent) {
            printf("\r[%s] 수신 중... %3d%% (%lld / %lld 바이트)",
                addr, percent, totalRecv, fileSize);
            fflush(stdout);
            lastPercent = percent;
        }
    }

    printf("\n[%s] 파일 수신 완료: %s (총 %lld 바이트)\n", addr, filename, totalRecv);

    fclose(fp);
    closesocket(client_sock);
    printf("[%s] 클라이언트 종료\n", addr);
    return 0;
}
int main(void)
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind(), listen()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    printf("[TCP 서버] 클라이언트 접속 대기 중...\n");

    while (1)
    {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);

        if (client_sock == INVALID_SOCKET) { err_display("accept()"); break; }

        // 스레드 생성
        HANDLE hThread = CreateThread(NULL,
            0,                   // 스택 크기 (0 = 기본)
            ClientThread,        // 스레드 함수
            (LPVOID)client_sock, // 스레드 함수에 전달할 인수
            0,                   // 스레드 생성 제어
            NULL);               // 스레드 ID

        if (hThread == NULL) {
            printf("[서버] 스레드 생성 실패\n");
            closesocket(client_sock);
        }
        else {
            CloseHandle(hThread); // 스레드 핸들 닫기
        }
    }

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
