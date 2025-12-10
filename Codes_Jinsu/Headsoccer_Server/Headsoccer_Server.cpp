#include "..\Common.h"
#include <thread>
#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_PLAYER 1

LONG PlayerNum = -1;                 // Interlocked를 위해 LONG 타입

//필요한 변수 선언
int sceneNum; //씬 번호
bool IsReady[MAX_PLAYER]; // 플레이어 준비 상태
bool AllReady; // 모든 플레이어 준비 상태
bool SceneNumPrintOnce[MAX_PLAYER] = {};
bool IsReadyPrintOnce[MAX_PLAYER] = {};


struct ThreadParam {
    SOCKET sock;
    int playerNum;
};

int GivePlayerNum(volatile LONG* p)
{
    // Thread-safe 순환 연산
    LONG old, next;
    do {
        old = *p;
        next = (old + 1) > 2 ? 0 : (old + 1);
    } while (InterlockedCompareExchange(p, next, old) != old);
    return (int)next;
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



    //키보드 입력 값, 점수, 좌표 등등 송수신에 필요한 변수 추가 선언 필요

    while (1) {
        //씬 넘버 송신
        retval = send(client_sock, (char*)&sceneNum, sizeof(int), 0);
        if (retval == SOCKET_ERROR) { err_display("send()"); }
        else {
            if (!SceneNumPrintOnce[myPlayerNum]) {
                printf("[서버] 플레이어 %d에게 씬 번호 전송: %d\n", myPlayerNum, sceneNum);
                SceneNumPrintOnce[myPlayerNum] = true; //한 번만 출력되도록 설정
			}
        }

        if (sceneNum == 2) {    //캐릭터 선택 씬
            // 플레이어 준비 상태 수신
            retval = recv(client_sock, (char*)&IsReady[myPlayerNum], sizeof(bool), MSG_WAITALL);
            if (retval == SOCKET_ERROR) { err_display("recv()"); }
            else {
                if(IsReady[myPlayerNum] != IsReadyPrintOnce[myPlayerNum]) {
                    printf("[서버] 플레이어 %d로부터 준비 상태 수신: %s\n", myPlayerNum, IsReady[myPlayerNum] ? "준비 완료" : "준비 미완료");
                    IsReadyPrintOnce[myPlayerNum] = IsReady[myPlayerNum]; //한 번만 출력되도록 설정
				}
			}
            
			//모든 플레이어의 준비 완료 상태 확인
			AllReady = true; // 일단 모든 플레이어가 준비 완료라고 가정
            for (int i = 0; i < MAX_PLAYER; i++) {
                if (!IsReady[i]) {
                    sceneNum = 2; //아직 준비 안된 플레이어가 있으면 캐릭터 선택 씬 유지
					AllReady = false; // AllReady도 false 유지
                    break;
				}
            }

            //모든 플레이어 준비 완료 상태 송신
            retval = send(client_sock, (char*)&AllReady, sizeof(bool), 0);
            if (retval == SOCKET_ERROR) { err_display("send()"); }

            if (AllReady) {
                printf("모든 플레이어 준비 완료\n");
                sceneNum = 3; //모든 플레이어가 준비 완료면 게임 시작 씬으로 변경
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
			//sceneNum = 4; //종료 씬으로 변경

        }

        else if (sceneNum == 4) {   //게임 종료 씬
            // 최종 점수 송신

            // (필요하면) 다시하기 버튼 누르면 
                // ball 위치 초기화
                // killTimer
                
            // 클라이언트 접속 종료
			printf("[서버] 플레이어 %d 접속 종료\n", myPlayerNum);
            
            //모든 플레이어의 준비 완료 상태 확인
            for (int i = 0; i < MAX_PLAYER; i++) {
                if (IsReady[i]) {
                    sceneNum = 4; // 게임을 종료하지 않은 플레이어가 있으면 씬 넘버 유지
                    break;
                }
				AllReady = false; // 모든 플레이어가 게임을 종료했으면 AllReady 초기화
                sceneNum = 2; //모든 플레이어가 게임을 종료했으면 씬 넘버 초기화
				printf("[서버] 모든 플레이어 접속 종료, 씬 번호 초기화\n");
            }
            break;
		}
    }
    
	printf("서버 스레드 종료\n");
    closesocket(client_sock);
    delete p;   // 할당된 구조체 메모리 해제

    return 0;
}


int main(void)
{
    int retval;
	sceneNum = 3; //캐릭터 선택 화면
	//AllReady = false; // 모든 플레이어 준비 상태 초기화

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

        DWORD optval = 1; // Nagle 알고리즘 비활성화
        setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));

        // 스레드 전달 구조체 생성
        ThreadParam* param = new ThreadParam;
        param->sock = client_sock;
        param->playerNum = GivePlayerNum(&PlayerNum);   // 고유 번호 부여

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
