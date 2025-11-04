#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

int main(int argc, char* argv[])
{
	int retval;

	if (argc < 3) {
		printf("사용법: %s <IPv4 주소> <보낼파일이름>\n", argv[0]);
		return 1;
	}

	const char* filename = argv[2];

	// 전체 경로에서 파일 이름만 추출
	const char* baseFilename = strrchr(filename, '\\'); // 마지막 '\' 위치 찾기
	if (baseFilename)
		baseFilename++; // '\' 다음부터가 실제 파일 이름
	else
		baseFilename = filename; // '\' 없으면 전체 이름 사용


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
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(SERVERPORT);

	// connect()
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	printf("[클라이언트] 서버 연결 성공!\n");

	// 파일 열기
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		perror("fopen()");
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// 파일 크기 계산
	_fseeki64(fp, 0, SEEK_END);
	long long fileSize = _ftelli64(fp);
	_fseeki64(fp, 0, SEEK_SET);

	// 파일 이름 길이 전송
	int nameLen = (int)strlen(baseFilename);
	retval = send(sock, (char*)&nameLen, sizeof(int), 0);
	if (retval == SOCKET_ERROR) err_quit("send() - nameLen");

	//파일 이름 전송
	retval = send(sock, baseFilename, nameLen, 0);
	if (retval == SOCKET_ERROR) err_quit("send() - filename");

	// 파일 크기 전송
	retval = send(sock, (char*)&fileSize, sizeof(long long), 0);
	if (retval == SOCKET_ERROR) err_quit("send() - fileSize");

	printf("[클라이언트] 파일 이름: %s\n", baseFilename);
	printf("[클라이언트] 파일 크기: %lld 바이트\n", fileSize);

	// 파일 내용 전송 (버퍼 단위)
	char buf[BUFSIZE];
	long long totalSent = 0;
	while (!feof(fp)) {
		size_t bytesRead = fread(buf, 1, BUFSIZE, fp);
		if (bytesRead > 0) {
			retval = send(sock, buf, (int)bytesRead, 0);
			if (retval == SOCKET_ERROR) {
				err_display("send() - file data");
				break;
			}
			totalSent += retval;
			printf("\r[클라이언트] 전송 중... %lld / %lld 바이트", totalSent, fileSize);
			fflush(stdout);
		}
	}

	printf("\n[클라이언트] 파일 전송 완료!\n");

	fclose(fp);
	closesocket(sock);
	WSACleanup();
	return 0;
}
