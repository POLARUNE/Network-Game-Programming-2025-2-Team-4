#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define SERVERIP   "127.0.0.1"

int main(int argc, char* argv[])
{
	int retval;

	// 플레이어 준비 상태 변수
	bool PlayerReady = false;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// 서버 주소 설정
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// connect()
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	printf("[클라이언트] 서버 연결 성공!\n");

	// 플레이어 준비 상태 전송
	retval = send(sock, (char*)&PlayerReady, sizeof(bool), 0);
	if (retval == SOCKET_ERROR) err_quit("send() - PlayerReady");
	printf("[클라이언트] 플레이어 준비 상태 전송 완료: %s\n", PlayerReady ? "준비됨" : "준비 안됨");

	closesocket(sock);
	WSACleanup();
	return 0;
}
