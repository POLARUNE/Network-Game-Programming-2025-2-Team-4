#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_PLAYER 3

LONG PlayerNum = -1;                 // Interlocked를 위해 LONG 타입
bool PlayerReady[MAX_PLAYER] = { false };

struct ThreadParam {
    SOCKET sock;
    int playerNum;
};

int GivePlayerNum()
{
    // Thread-safe 증가
    return (int)InterlockedIncrement(&PlayerNum);
}

DWORD WINAPI RecvFromClient(LPVOID arg)
{
    ThreadParam* p = (ThreadParam*)arg;
    SOCKET client_sock = p->sock;
    int myPlayerNum = p->playerNum;

    int retval;

    // 클라이언트 주소 출력
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);

    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

    printf("\n[TCP 서버] 클라이언트 접속 - IP 주소: %s, 포트 번호: %d, 플레이어 번호: %d\n",
        addr, ntohs(clientaddr.sin_port), myPlayerNum);

    // 플레이어 준비 상태 수신
    retval = recv(client_sock, (char*)&PlayerReady[myPlayerNum], sizeof(bool), MSG_WAITALL);

    printf("[서버] 플레이어 %d 준비 상태: %s\n", myPlayerNum, PlayerReady[myPlayerNum] ? "준비 완료" : "준비 미완료");

    closesocket(client_sock);
    delete p;   // 할당된 구조체 메모리 해제

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
    if (listen_sock == INVALID_SOCKET)
        err_quit("socket()");

    // bind(), listen()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR)
        err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR)
        err_quit("listen()");

    printf("[TCP 서버] 클라이언트 접속 대기 중...\n");

    while (1)
    {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);

        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // 스레드 전달 구조체 생성
        ThreadParam* param = new ThreadParam;
        param->sock = client_sock;
        param->playerNum = GivePlayerNum();   // 고유 번호 부여

        // 스레드 생성
        HANDLE hThread = CreateThread(NULL, 0, RecvFromClient, param, 0, NULL);

        if (hThread == NULL) {
            printf("[서버] 스레드 생성 실패\n");
            closesocket(client_sock);
            delete param;  // 메모리 해제
        }
        else {
            CloseHandle(hThread); // 스레드 핸들 닫기
        }
    }

    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
