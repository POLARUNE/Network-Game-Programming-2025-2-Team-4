#include "..\Common.h"
#include "CHARACTER.h"
#include "Korea.h"

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

    // 플레이어 준비 상태 수신
    bool PlayerReady;
    retval = recv(client_sock, (char*)&PlayerReady, sizeof(bool), MSG_WAITALL);
	if (retval <= 0) { err_display("recv() - PlayerReady"); closesocket(client_sock); return 1; }

	printf("[서버] 플레이어 준비 상태: %s\n", PlayerReady ? "준비 완료" : "준비 미완료");

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
