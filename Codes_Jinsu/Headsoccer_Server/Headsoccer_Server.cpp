#include "..\Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_PLAYER 3

LONG PlayerNum = -1;                 // Interlocked를 위해 LONG 타입

int sceneNum; //씬 번호

struct ThreadParam {
    SOCKET sock;
    int playerNum;
};

int GivePlayerNum()
{
    // Thread-safe 증가
    return (int)InterlockedIncrement(&PlayerNum);
}

DWORD WINAPI CommunicateToClient(LPVOID arg) // 클라이언트 CommunicateToServer 스레드 함수와 통신
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

    //필요한 변수 선언
	bool PlayerReady; // 플레이어 준비 상태
	bool AllplayerReady; // 모든 플레이어 준비 상태
    //키보드 입력 값, 점수, 좌표 등등 송수신에 필요한 변수 추가 선언 필요

    while (1) {
        if (sceneNum == 2) {    //캐릭터 선택 씬
            // 플레이어 준비 상태 수신
            retval = recv(client_sock, (char*)&PlayerReady, sizeof(bool), MSG_WAITALL);

            printf("[서버] 플레이어 %d 준비 상태: %s\n", myPlayerNum, PlayerReady ? "준비 완료" : "준비 미완료");

			//모든 플레이어의 준비 완료 상태 확인 (여기서는 간단히 처리)

			if (PlayerReady) { //모든 플레이어가 준비 완료 상태라고 가정
                sceneNum = 3; //게임 시작 씬으로 변경
            }
            //씬 넘버 송신
            retval = send(client_sock, (char*)&sceneNum, sizeof(int), 0);
            if (retval == SOCKET_ERROR) { err_display("send()"); }
            else {
                printf("[서버] 플레이어 %d에게 씬 번호 전송: %d\n", myPlayerNum, sceneNum);
            }
        }

        else if (sceneNum == 3) {   //게임 시작 씬
            // 키보드 입력 값 수신
            // 공, 플레이어들 좌표 계산
            // 공, 플레이어들 좌표 송신

            //만약 골이 먹혔으면
                //해당 플레이어 점수 +1
            //점수 송신

            //게임 시간 송신

            //만약 게임 시간이 종료되었으면 (클라이언트 WM_TIMER 부분 참고)
			//sceneNum = 4; 종료 씬으로 변경

            //테스트 코드
			//6초 기다리고 바로 종료 씬으로 변경
			Sleep(6000);
			sceneNum = 4;
        }

        else if (sceneNum == 4) {   //게임 종료 씬
            // 최종 점수 송신

            // (필요하면) 다시하기 버튼 누르면 
                // ball 위치 초기화
                // killTimer
                
            // 게임 종료하면
            // 클라이언트 접속 종료
			printf("[서버] 플레이어 %d 접속 종료\n", myPlayerNum);
            break;
		}
    }
 
    closesocket(client_sock);
    delete p;   // 할당된 구조체 메모리 해제

    return 0;
}


int main(void)
{
    int retval;
	sceneNum = 2; //캐릭터 선택 화면

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
        HANDLE hThread = CreateThread(NULL, 0, CommunicateToClient, param, 0, NULL);

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
