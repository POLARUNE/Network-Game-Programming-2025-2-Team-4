#include "..\Common.h"
#include "Player.h"
#include <atomic>
#include <iostream>

#define SERVERPORT 9000
#define BUFSIZE    512
#define FRICTION 0.8f
#define GRAVITY 9.8f*2
#define SHOOT 300

int sceneNum;                        // 씬 번호

// 슬롯 기반 플레이어 관리 (번호 재사용)
static std::atomic<bool> playerConnected[MAX_PLAYER]; // false = 빈 슬롯

//atomic: thread 여러 개가 동시에 접근해도 안전하게 값을 읽고 쓸 수 있도록 보장
std::atomic<int> connectedPlayerCount = 0; // 현재 접속한 플레이어 수
std::atomic<int> readyPlayerCount = 0;     // 준비 완료한 플레이어 수
std::atomic<bool> AllplayerReady = false; // 모든 플레이어 준비 완료 플래그

// 이벤트은 main에서 생성/해제
HANDLE gameStartEvent = NULL;   // 게임 시작 이벤트
HANDLE sendRequestEvent = NULL; // 서버가 상태 계산 완료 알림 이벤트

struct ThreadParam {
    SOCKET sock;
    int playerNum;
};

Player players[MAX_PLAYER]{ {false, 100, 630}, {false, 875, 630}, {false, 400, 630} }; // 플레이어 좌표 초기화
BallPacket ball{ 500, 300 };

// 빈 슬롯을 찾아서 번호 할당
int GivePlayerNum()
{
    for (int i = 0; i < MAX_PLAYER; ++i) {
        bool expected = false;
        if (playerConnected[i].compare_exchange_strong(expected, true)) {
            // 성공적으로 슬롯 획득
            return i;
        }
        // else: expected was true, 다른 스레드가 사용 중
    }
    return -1; // 빈 슬롯 없음
}

// 통신 스레드
DWORD WINAPI CommunicateToClient(LPVOID arg)
{
    ThreadParam* p = (ThreadParam*)arg;
    SOCKET client_sock = p->sock;
    int myPlayerNum = p->playerNum;
    int retval = 0;

    // (옵션) 클라이언트 주소 출력
    struct sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    if (getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen) == 0) {
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP 서버] 클라이언트 접속 - IP: %s, 포트: %d, 플레이어 번호: %d\n",
            addr, ntohs(clientaddr.sin_port), myPlayerNum);
    }

    // ---- 준비(READY) 받기 ----
    bool PlayerReady = false;
    retval = recv(client_sock, (char*)&PlayerReady, sizeof(bool), MSG_WAITALL);
    if (retval == SOCKET_ERROR || retval == 0) {
        printf("[서버] 플레이어 %d의 READY 수신 실패/종료\n", myPlayerNum);
        closesocket(client_sock);
        // 슬롯 반환
        playerConnected[myPlayerNum].store(false);
        delete p;
        return 0;
    }

    printf("[서버] 플레이어 %d 준비 상태: %s\n", myPlayerNum, PlayerReady ? "준비 완료" : "준비 미완료");

    // READY 신호를 보낸 플레이어 수 증가 (true인 경우에만)
    if (PlayerReady) {
        int newReady = readyPlayerCount.fetch_add(1) + 1; // 증가 후 값
        printf("[서버] 현재 준비 완료 플레이어 수: %d\n", newReady);

        // 마지막 플레이어가 준비되면 게임 시작 이벤트 설정
        if (newReady == MAX_PLAYER) {
            AllplayerReady.store(true);
            sceneNum = 3; // 게임 씬
            SetEvent(gameStartEvent);
            printf("[서버] 모든 플레이어 준비 완료 - 게임 시작 이벤트 Set\n");
        }
    }

    // ---- 모든 플레이어 준비될 때까지 대기 ----
    WaitForSingleObject(gameStartEvent, INFINITE);

    // 씬 번호 송신
    retval = send(client_sock, (char*)&sceneNum, sizeof(int), 0);
    if (retval == SOCKET_ERROR) {
        err_display("send()");
        closesocket(client_sock);
        playerConnected[myPlayerNum].store(false);
        delete p;
        return 0;
    }
    else {
        printf("[서버] 플레이어 %d에게 씬 번호 전송: %d\n", myPlayerNum, sceneNum);
    }

    // ---- 게임 루프 (클라이언트와의 반복 송수신) ----
    while (AllplayerReady.load()) {
        if (sceneNum == 3) {   // 게임 씬일 때
            // 클라이언트로부터 입력 수신
            retval = recv(client_sock, &(players[myPlayerNum].inputFlag), sizeof(char), MSG_WAITALL);
            if (retval == SOCKET_ERROR || retval == 0) {
                printf("[서버] 플레이어 %d 입력 수신 실패/종료\n", myPlayerNum);
                break;
            }

            // 서버 측에서 게임 상태 계산이 완료될 때까지 대기
            WaitForSingleObject(sendRequestEvent, INFINITE);

            // 플레이어들 및 공 상태 전송
            retval = send(client_sock, (char*)players, sizeof(Player) * MAX_PLAYER, 0);
            if (retval == SOCKET_ERROR) {
                printf("[서버] 플레이어 %d에게 players 전송 실패\n", myPlayerNum);
                break;
            }

            retval = send(client_sock, (char*)&ball, sizeof(BallPacket), 0);
            if (retval == SOCKET_ERROR) {
                printf("[서버] 플레이어 %d에게 ball 전송 실패\n", myPlayerNum);
                break;
            }

            // (점수/게임 시간 등 추가 전송 가능)
        }
        else {
            // 다른 씬 처리 (필요시)
            Sleep(10);
        }
    }

    // 스레드 정리: 연결 닫기, 슬롯 반환, connected count 감소, 메모리 해제
    closesocket(client_sock);
    playerConnected[myPlayerNum].store(false);
    connectedPlayerCount.fetch_sub(1);
    printf("[서버] 플레이어 %d 연결 종료. 현재 접속수: %d\n", myPlayerNum, connectedPlayerCount.load());

    delete p; // param 메모리 해제 (한 번만)
    return 0;
}

int main(void)
{
    int retval;
    sceneNum = 2; // 캐릭터 선택 화면

    // 이벤트 생성 (main에서)
    gameStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // manual-reset, initially not signaled
    sendRequestEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // playerConnected 초기화
    for (int i = 0; i < MAX_PLAYER; ++i) playerConnected[i].store(false);

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

    // ---- 클라이언트 접속 처리 루프 (MAX_PLAYER만큼 받음) ----
    while (true)
    {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);
        SOCKET client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);

        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // 빈 슬롯 찾아서 번호 할당
        int assignedNum = GivePlayerNum();
        if (assignedNum == -1) {
            printf("[서버] 플레이어 수 초과. 연결 거부\n");
            closesocket(client_sock);
            continue;
        }

        // Nagle 알고리즘 비활성화 (옵션)
        DWORD optval = 1;
        setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));

        // 스레드 전달 구조체 생성
        ThreadParam* param = new ThreadParam;
        param->sock = client_sock;
        param->playerNum = assignedNum;

        // 스레드 생성
        HANDLE hThread = CreateThread(NULL, 0, CommunicateToClient, param, 0, NULL);
        if (hThread == NULL) {
            printf("[서버] 스레드 생성 실패 (player %d)\n", assignedNum);
            closesocket(client_sock);
            // 슬롯 반환
            playerConnected[assignedNum].store(false);
            delete param;  // 스레드 생성 실패 시 메모리 해제
            continue;
        }
        else {
            CloseHandle(hThread); // 핸들만 닫음 (스레드는 계속 동작)
            int newConn = connectedPlayerCount.fetch_add(1) + 1;
            printf("[서버] 현재 접속 플레이어 수: %d\n", newConn);

            // 모든 플레이어가 접속하면 accept 루프 종료하여 메인 루프(게임 루프)로 진입
            if (newConn == MAX_PLAYER) {
                printf("[서버] 모든 플레이어 접속 완료. 게임 루프 진입 준비.\n");
                break;
            }
        }
    }

    // ---- 게임 루프 ----
    float yVel = 0.5f;
    float xVel = 0;
    float deltaTime = 0.017f;

    while (true) {
        Sleep(17); // 약 60FPS

        // 매 프레임, sendRequestEvent를 reset -> 계산 끝나면 SetEvent
        ResetEvent(sendRequestEvent);

        // 플레이어들 입력 적용
        for (int i = 0; i < MAX_PLAYER; ++i)
        {
            if (players[i].inputFlag & 1) {
                players[i].x -= 5;
            }
            if (players[i].inputFlag & 2) {
                players[i].x += 5;
            }
        }

        // 물리 처리 (볼)
        yVel += GRAVITY * deltaTime; // 중력은 시간 간격 반영
        ball.x += xVel * deltaTime;
        ball.y += yVel * deltaTime;

        // 공이 화면 밖으로 벗어나지 않게 처리
        if (ball.y + (15 * 2) >= 678)
        {
            ball.y = 678 - (15 * 2);
            yVel = -(yVel * FRICTION);
        }

        if (ball.y + (15 * 2) >= 570 && ball.y + (15 * 2) <= 600)
        {
            if (ball.x <= 92 || ball.x + (15 * 2) >= 930)
            {
                yVel = -(yVel * FRICTION);
            }
        }

        if (ball.y <= 170)
        {
            yVel = 0;
        }

        // 플레이어와의 충돌 처리 (샘플)
        RECT pext[MAX_PLAYER];
        for (int i = 0; i < MAX_PLAYER; ++i) {
            pext[i] = { players[i].x, players[i].y, players[i].x + 50, players[i].y + 50 };
        }

        // 단순 충돌/튕김 로직 (원래 코드 로직 유지)
        for (int i = 0; i < MAX_PLAYER; ++i) {
            if ((ball.y + (15 * 2)) >= pext[i].top && (ball.y + (15 * 2)) <= pext[i].top + 20) {
                if ((ball.x >= pext[i].left && ball.x <= pext[i].right) ||
                    (ball.x + (15 * 2) >= pext[i].left && ball.x + (15 * 2) <= pext[i].right))
                {
                    yVel = -(yVel + SHOOT);
                }
            }
            if ((ball.y + (15 * 2)) >= pext[i].top + 20) {
                if (ball.x <= pext[i].right && ball.x >= pext[i].left) {
                    xVel = SHOOT;
                }
                else if (ball.x + (15 * 2) >= pext[i].left && ball.x + (15 * 2) <= pext[i].right) {
                    xVel = -SHOOT;
                }
            }
        }

        if (ball.y <= 570) {
            if (ball.x <= 0 || ball.x >= 779) {
                xVel = -(xVel * FRICTION);
            }
        }

        // 모든 프레임 계산 완료 → 스레드들에 전송 가능 알림
        SetEvent(sendRequestEvent);

        // 게임 종료 조건 체크
        // if (시간이 종료되면) { sceneNum = 4; break; }
    }

    // 정리
    CloseHandle(gameStartEvent);
    CloseHandle(sendRequestEvent);
    closesocket(listen_sock);
    WSACleanup();

    return 0;
}
