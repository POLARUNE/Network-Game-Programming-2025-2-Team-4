#include "../Common.h"
#include "CHARACTER.h"
#include "Korea.h"
#include "Brazil.h"
#include "Canada.h"

#define SERVERPORT	9000
#define BUFSIZE		128

#define MAX_CLI 3

RECT P1Rect;
RECT P2Rect;
BOOL CrashCheck;

LONG PlayerNum = 0; // InterLocked를 위한 LONG 타입

bool IsReady[MAX_CLI] = { false };
bool AllReady = false;

struct ThreadParam {
	SOCKET sock;
	int PlayerNum;
};

int GivePlayerNum()
{
	// Thread-safe 증가
	return (int)InterlockedIncrement(&PlayerNum);
}

// 클라이언트와 데이터 통신
DWORD WINAPI ClientThread(LPVOID arg)
{
	// 데이터 통신에 사용할 변수
	ThreadParam* p = (ThreadParam*)arg;
	SOCKET client_sock = p->sock;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;

	int retval;
	int myPNum = p->PlayerNum;

	// 클라이언트 정보 받기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	while (1)
	{
		// 클라이언트와 통신
		retval = recv(client_sock, (char*)&IsReady[myPNum], sizeof(bool), MSG_WAITALL);
		if (retval <= 0) err_quit("recv()");

		printf("[TCP %s] 클라이언트%d %s\n", addr, myPNum, IsReady[myPNum] ? "준비 완료" : "준비 대기 중");

		if (IsReady[myPNum]) AllReady = true;
		else AllReady = false;

		retval = recv(client_sock, (char*)&AllReady, sizeof(bool), MSG_WAITALL);
		if (retval <= 0) err_quit("recv()");
		else printf("모든 클라이언트 %s\n", AllReady ? "준비 완료" : "준비 대기 중");

	}

	// 소켓 닫기
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));
	delete p;
	return 0;
}

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

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

	printf("[서버] 클라이언트 접속 대기 중...\n");

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// 스레드 전달 구조체 생성
		ThreadParam* param = new ThreadParam;
		param->sock = client_sock;
		param->PlayerNum = GivePlayerNum();   // 고유 번호 부여

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ClientThread, param, 0, NULL);
		if (hThread == NULL)
		{
			printf("[서버] 스레드 생성 실패\n");
			closesocket(client_sock);
			delete param;
		}
		else CloseHandle(hThread);
	}

	// 소켓 닫기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}