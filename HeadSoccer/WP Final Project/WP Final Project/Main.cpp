#include"..\..\Common.h"
#include <atlimage.h>
#include <math.h>


#include "BACKGROUND.h"
#include "BALL.h"
#include "CHARACTER.h"
#include "Alien.h"
#include "Asura.h"
#include "Brazil.h"
#include "Korea.h"
#include "Cameroon.h"
#include "Canada.h"
#include "Egypt.h"
#include "Israel.h"
#include "Italy.h"
#include "Poland.h"
#include "../../Headsoccer_Server/player.h"
#include "resource.h"

#pragma comment (lib, "msimg32.lib")
#pragma comment (lib, "winmm.lib")

#define SERVERPORT 9000
#define BUFSIZE    512
#define SERVERIP   "192.168.0.1"

HINSTANCE g_hInst;
TCHAR lpszClass[] = TEXT("HEAD SOCCER");

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dialog_Proc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI CommunicateToServer(LPVOID arg);

void DebugLog(const char* message, ...);
void LOOP(HWND, BOOL KB[]);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	if (!hPrevInstance) {
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = hInstance;
		WndClass.lpfnWndProc = (WNDPROC)WndProc;
		WndClass.lpszClassName = lpszClass;
		WndClass.lpszMenuName = NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);
	}
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 1016, 779, NULL, (HMENU)NULL, hInstance, NULL);		//--- 윈도우 크기 변경 가능
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	WSACleanup();
	return Message.wParam;
}

CImage BackGround;
CImage GoalPostR, GoalPostL;

RECT WinSize;

int Timer_M = 1;
int Timer_S = 0;

TCHAR Timer[10];

Character* P1;
Character* P2;
Character* P3;
Ball ball;

BOOL CrashCheck = FALSE;

RECT P1Rect, P2Rect;

int P1Num, P2Num;
BOOL Kick1, Kick2;
BOOL Goal1, Goal2;

BOOL Pause = FALSE;
int SceneNum = 0;
BOOL PlayerReady = FALSE;

CImage Char[2][10];
CImage CharP1;
CImage CharP2;

CImage ResBG;

BOOL P1Power, P2Power;
BOOL P1Crash, P2Crash;
BOOL KeyBuffer[256] = { FALSE };

HANDLE enterEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // 캐릭터선택 -> 인게임 넘어갈 때까지 동기화 필요

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memdc;
	PAINTSTRUCT ps;

	static HBITMAP hBitmap;

	static CImage StartBG;
	static CImage CharSelectBG;
	static CImage ResultBG;

	static int JmpCnt1, JmpCnt2;

	RECT CrashSize;

	static RECT ButtonPlay;
	static RECT ButtonExit;

	static POINT mouse;

	static RECT CharSelRect[10];

	static int P1Score, P2Score;
	switch (iMessage) {
	case WM_CREATE:
		SceneNum = 1;

		ButtonPlay = { 120, 650, 360, 700 };
		ButtonExit = { 680, 655, 930, 700 };

		P1Num = P2Num = 0;

		StartBG.Load(_T("sprite\\StartBG.png"));
		CharSelectBG.Load(_T("sprite\\CharSelectBG.png"));
		BackGround.Load(_T("sprite\\BackGround.png"));
		GoalPostR.Load(_T("sprite\\GoalPost - R.png"));
		GoalPostL.Load(_T("sprite\\GoalPost - L.png"));

		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 5; ++j) {
				CharSelRect[j + (i * 5)] = { 250 + (j * 100), 30 + (i * 120), 350 + (j * 100), 130 + (i * 120) };
			}
		}

		GetClientRect(hWnd, &WinSize);

		Timer_M = 1;
		Timer_S = 0;

		JmpCnt1 = JmpCnt2 = 0;
		Kick1 = Kick2 = Goal1 = Goal2 = FALSE;

		PlaySound(L"sound\\bgm.wav", NULL, SND_ASYNC | SND_LOOP);

		break;

	case WM_KEYDOWN:
		KeyBuffer[wParam] = TRUE;
		/*case VK_ESCAPE:
			if (SceneNum == 3) {
				Pause = TRUE;
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)Dialog_Proc);
			}

			break; */
		break;

/*		InvalidateRect(hWnd, NULL, FALSE);

		break;*/

	case WM_KEYUP:
		// Record key up state
		if (wParam >= 0 && wParam < 256) {
			KeyBuffer[wParam] = FALSE;
		}
		break;

	case WM_TIMER:
		switch (LOWORD(wParam)) {
		case 1:
			if (!Pause) {
				if (Timer_M == 1) {
					Timer_M = 0;
					Timer_S = 100;
				}

				else if (Timer_M == 0) {
					if (--Timer_S == 0) {
						KillTimer(hWnd, 1);
						KillTimer(hWnd, 2);
						KillTimer(hWnd, 5);
						PlaySound(L"sound\\whistle.wav", NULL, SND_ASYNC);
						SceneNum = 4; // 서버에서 씬 변환 및 타이머 관리 필요
					}
				}
			}

			break;

		case 2:
			if (JmpCnt1 <= 8) {
				P1->Jump(1);
			}

			else {
				P1->Jump(0);
			}

			if (JmpCnt1++ == 17) {
				KillTimer(hWnd, 2);
				JmpCnt1 = 0;
			}

			break;

		case 3:
			if (JmpCnt2 <= 8) {
				P2->Jump(1);
			}

			else {
				P2->Jump(0);
			}

			if (JmpCnt2++ == 17) {
				KillTimer(hWnd, 3);
				JmpCnt2 = 0;
			}

			break;

		case 4:
			if (!Pause) {
				P1->PwGaugeFull();
				P2->PwGaugeFull();
			}

			break;

		case 5:
			ball.Physics(P1, P2);

			if (Goal1)
			{
				KillTimer(hWnd, 2);
				KillTimer(hWnd, 3);
				P1->Goaled();
				P2->Goal();
				P1->ResetPos(1);
				P2->ResetPos(2);
				P3->ResetPos(3);
				ball.Reset();
				Goal1 = FALSE;
				P1Power = FALSE;
				P2Power = FALSE;
				P1Crash = FALSE;
				P2Crash = FALSE;
				PlaySound(L"sound\\bgm.wav", NULL, SND_ASYNC | SND_LOOP);
			}

			else if (Goal2)
			{
				KillTimer(hWnd, 2);
				KillTimer(hWnd, 3);
				P1->Goal();
				P2->Goaled();
				P1->ResetPos(1);
				P2->ResetPos(2);
				P3->ResetPos(3);
				ball.Reset();
				Goal2 = FALSE;
				P1Power = FALSE;
				P2Power = FALSE;
				P1Crash = FALSE;
				P2Crash = FALSE;
				PlaySound(L"sound\\bgm.wav", NULL, SND_ASYNC | SND_LOOP);
			}

			break;

		case 6:
			P1->Kick(1);
			Kick1 = FALSE;
			KillTimer(hWnd, 6);
			break;

		case 7:
			P2->Kick(2);
			Kick2 = FALSE;
			KillTimer(hWnd, 7);
			break;
		}

		InvalidateRect(hWnd, NULL, FALSE);

		break;

	case WM_LBUTTONDOWN:
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);

		switch (SceneNum) {
		case 1:
			if (PtInRect(&ButtonPlay, mouse)) {
				SceneNum = 2;
			}

			else if (PtInRect(&ButtonExit, mouse)) {
				PostQuitMessage(0);
			}

			break;

		case 2:
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 5; ++j) {
					if (PtInRect(&CharSelRect[j + (i * 5)], mouse)) {
						P1Num = j + (i * 5);
					}
				}
			}

			switch (P1Num) {
			case 0: // p1
				P1 = new Alien(1);
				break;
			case 1:
				P1 = new Asura(1);
				break;
			case 2:
				P1 = new Brazil(1);
				break;
			case 3:
				P1 = new Cameroon(1);
				break;
			case 4: //p2
				P1 = new Canada(1);
				break;
			case 5:
				P1 = new Egypt(1);
				break;
			case 6:
				P1 = new Israel(1);
				break;
			case 7:
				P1 = new Italy(1);
				break;
			case 8:
				P1 = new Korea(1);
				break;
			case 9:
				P1 = new Poland(1);
				break;
			}

			P2 = new Canada(2); // 임시로 2P 캐릭터 캐나다로 설정
			P3 = new Italy(3);	// 임시로 3P 캐릭터 캐나다로 설정

			if (sqrt(pow(500 - mouse.x, 2) + pow(530 - mouse.y, 2)) <= 110) {
				PlayerReady = TRUE;
				CreateThread(NULL, 0, CommunicateToServer, NULL, 0, NULL); // 서버와 통신 시작

				//SceneNum = 3; // 서버에서 씬 변환하므로 주석 처리
				DeleteSelBG();
				SetTimer(hWnd, 1, 1000, NULL);
				SetTimer(hWnd, 4, 1000, NULL);
				SetTimer(hWnd, 5, 10, NULL);
			}
			// 서버에서 씬정보 받을때까지 대기
			WaitForSingleObject(enterEvent, INFINITE);
			break;

		case 4:
			if (sqrt(pow(500 - mouse.x, 2) + pow(320 - mouse.y, 2)) <= 100) { //다시하기 버튼(필요하면 서버에서 구현해야됨.)
				PlayerReady = FALSE;
				SceneNum = 2;

				//이하 부분은 서버에서 처리 필요
				Timer_M = 1;
				Timer_S = 0;
				ball.Reset();
				DeleteResBG();
				delete P1;
				delete P2;
				delete P3;
				KillTimer(hWnd, 2);
				KillTimer(hWnd, 3);
				KillTimer(hWnd, 4);
				KillTimer(hWnd, 5);
				PlaySound(L"sound\\bgm.wav", NULL, SND_ASYNC | SND_LOOP);
			}

			else if (sqrt(pow(500 - mouse.x, 2) + pow(570 - mouse.y, 2)) <= 100) {
				DeleteResBG();
				delete P1;
				delete P2;
				delete P3;
				StartBG.Destroy();
				CharSelectBG.Destroy();
				BackGround.Destroy();
				GoalPostR.Destroy();
				GoalPostL.Destroy();
				PostQuitMessage(0);
			}

			break;
		}

		InvalidateRect(hWnd, NULL, FALSE);

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hBitmap = CreateCompatibleBitmap(hdc, WinSize.right, WinSize.bottom);
		memdc = CreateCompatibleDC(hdc);

		(HBITMAP)SelectObject(memdc, hBitmap);

		switch (SceneNum) {
		case 1:
			StartBG.BitBlt(memdc, 0, 0, 1000, 740, 0, 0, SRCCOPY);
			break;

		case 2:
			CharSelectBG.StretchBlt(memdc, WinSize, SRCCOPY);
			DrawSelectBG(memdc, 0, 0);
			break;

		case 3:
			DrawBG(memdc);

			P1->UI_Print(memdc, 1);
			P2->UI_Print(memdc, 2);

			P1Rect = P1->CharPos();
			P2Rect = P2->CharPos();

			CrashCheck = IntersectRect(&CrashSize, &P1Rect, &P2Rect);

			ball.Draw(memdc);

			if (!Pause) {
				LOOP(hWnd, KeyBuffer);
			}

			if (P1Crash) {
				P1->PowerShoot(memdc, 1, ball.BallxPos(), ball.BallyPos());
			}

			if (P2Crash) {
				P2->PowerShoot(memdc, 2, ball.BallxPos(), ball.BallyPos());
			}

			P1->Draw(memdc, 1);
			P2->Draw(memdc, 2);
			P3->Draw(memdc, 2);

			break;

		case 4:
			P1Score = P1->CharScore();
			P2Score = P2->CharScore();

			DrawSelectBG(memdc, P1Score, P2Score);

			break;
		}

		BitBlt(hdc, 0, 0, 1000, 800, memdc, 0, 0, SRCCOPY);

		DeleteDC(memdc);
		EndPaint(hWnd, &ps);

		break;

	case WM_DESTROY:
		DeleteObject(hBitmap);
		DeleteResBG();
		delete P1;
		delete P2;
		StartBG.Destroy();
		CharSelectBG.Destroy();
		BackGround.Destroy();
		GoalPostR.Destroy();
		GoalPostL.Destroy();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void LOOP(HWND hWnd, BOOL KB[]) {
	/*if (KB[VK_LEFT])
	{
		P2->Move(1,2);
	}

	if (KB[VK_RIGHT])
	{
		P2->Move(2,2);
	}
	
	if (KB['a'] || KB['A'])
	{
		P1->Move(1,1);
	}

	if (KB['d'] || KB['D'])
	{
		P1->Move(2,1);
	}
	
	if (KB['w'] || KB['W'])
	{
		SetTimer(hWnd, 2, 25, NULL);
	}*/

	if (KB[VK_UP])
	{
		SetTimer(hWnd, 3, 25, NULL);
	}

	if (KB['s'] || KB['S'] && !Kick1)
	{
		P1->Kick(1);
		Kick1 = TRUE;
		SetTimer(hWnd, 6, 1000, NULL);
	}

	if (KB[VK_DOWN] && !Kick2)
	{
		P2->Kick(2);
		Kick2 = TRUE;
		SetTimer(hWnd, 7, 1000, NULL);
	}

	if (KB['f'] || KB['F'] && P1->PowerGauge() == 100) {
		P1Power = TRUE;
	}

	if (KB[VK_RETURN] && P2->PowerGauge() == 100) {
		P2Power = TRUE;
	}
}

BOOL CALLBACK Dialog_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg) {
	case WM_COMMAND:
		switch (wParam) {
		case IDOK:
			Pause = FALSE;
			EndDialog(hDlg, 0);
			break;

		case IDCANCEL:
			PostQuitMessage(0);
			EndDialog(hDlg, 0);
			break;
		}
	}

	return FALSE;
}

// 서버와 통신하는 스레드 함수
DWORD WINAPI CommunicateToServer(LPVOID arg) // 서버 CommunicateToClient 스레드 함수와 통신
{
	int retval;

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	DWORD optval = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&optval, sizeof(optval));

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

	Player players[MAX_PLAYER];
	BallPacket ballPacket{ 500, 300 };

	if (SceneNum == 2) {
		//플레이어 준비 상태 송신
		retval = send(sock, (char*)&PlayerReady, sizeof(bool), 0);
		if (retval == SOCKET_ERROR) err_quit("send() - PlayerReady");
		//printf("[클라이언트] 플레이어 준비 상태 전송 완료: %s\n", PlayerReady ? "준비됨" : "준비 안됨");

		// 씬 넘버 수신
		retval = recv(sock, (char*)&SceneNum, sizeof(int), 0);
		if (retval == SOCKET_ERROR) err_quit("recv() - SceneNum");
		SetEvent(enterEvent);
	}
	DebugLog("시작\n");

	while (1) {
		if (SceneNum == 3) {
			//키보드 입력 값 송신
			char inputFlag = 0; // 각비트별로 입력값을 나타냄
            if (KeyBuffer['A'] || KeyBuffer['a'] || KeyBuffer[VK_LEFT]) {
				inputFlag |= 1; 
			}
            if (KeyBuffer['D'] || KeyBuffer['d'] || KeyBuffer[VK_RIGHT]) {
				inputFlag |= 2;
			}
            send(sock, &inputFlag, sizeof(inputFlag), 0);
			
			
			//공, 플레이어들 좌표 수신
			recv(sock, (char*)players, sizeof(Player) * MAX_PLAYER, MSG_WAITALL); // inputFlag값 무시
			P1->SetPos(players[0].x, players[0].y);
			P2->SetPos(players[1].x, players[1].y);
			P3->SetPos(players[2].x, players[2].y);
			recv(sock, (char*)&ballPacket, sizeof(BallPacket), MSG_WAITALL);
			ball.SetPos(ballPacket.x, ballPacket.y);
			//점수 수신
			//게임 시간 수신
		}

		else if (SceneNum == 4) {
			// 최종 점수 수신
			
			//서버와 통신 종료
			closesocket(sock);
			return 0;
		}
		else break;
	}

	closesocket(sock);
	return 0;
}



void DebugLog(const char* format, ...) {
	char buffer[512];
	va_list args;
	va_start(args, format);
	_vsnprintf_s(buffer, 512, 512 - 1, format, args);
	va_end(args);
	OutputDebugStringA(buffer);
}