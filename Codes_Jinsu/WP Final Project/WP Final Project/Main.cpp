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
#include "resource.h"

#pragma comment (lib, "msimg32.lib")
#pragma comment (lib, "winmm.lib")

#define SERVERPORT 9000
#define BUFSIZE    512
#define SERVERIP   "127.0.0.1"

HINSTANCE g_hInst;
TCHAR lpszClass[] = TEXT("HEAD SOCCER");

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dialog_Proc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI SendToServer(LPVOID arg);

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memdc;
	PAINTSTRUCT ps;

	static HBITMAP hBitmap;

	static CImage StartBG;
	static CImage CharSelectBG;
	static CImage ResultBG;

	static BOOL KeyBuffer[256] = {FALSE};

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
		switch (wParam) {
		case 'a': 
		case 'A': // 1p 좌
		case 'd': 
		case 'D': // 1p 우
		case 'w':
		case 'W':
		case 's':
		case 'S':
		case 'f':
		case 'F':
		case VK_LEFT: // 2p 좌
		case VK_RIGHT: // 2p 우
		case VK_UP:
		case VK_DOWN:
		case VK_RETURN:
			KeyBuffer[wParam] = TRUE;
			break;

		case VK_ESCAPE:
			if (SceneNum == 3) {
				Pause = TRUE;
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)Dialog_Proc);
			}

			break;
		}

		InvalidateRect(hWnd, NULL, FALSE);

		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case 'a':
		case 'A': // 1p 좌
		case 'd':
		case 'D': // 1p 우
		case 'w':
		case 'W':
		case 's':
		case 'S':
		case 'f':
		case 'F':
		case VK_LEFT: // 2p 좌
		case VK_RIGHT: // 2p 우
		case VK_UP:
		case VK_DOWN:
		case VK_RETURN:
			KeyBuffer[wParam] = FALSE;
			break;
		}
		
		break;

	case WM_TIMER:
		switch (LOWORD(wParam)) {
		case 1:
			if (!Pause) {
				if (Timer_M == 1) {
					Timer_M = 0;
					Timer_S = 9;
				}

				else if (Timer_M == 0) {
					if (--Timer_S == 0) {
						KillTimer(hWnd, 1);
						KillTimer(hWnd, 2);
						KillTimer(hWnd, 5);
						PlaySound(L"sound\\whistle.wav", NULL, SND_ASYNC);
						SceneNum = 4;
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
			case 0:
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
			case 4:
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

			if (sqrt(pow(500 - mouse.x, 2) + pow(530 - mouse.y, 2)) <= 110) { // 준비 버튼을 누르면
				PlayerReady = TRUE;
				CreateThread(NULL, 0, SendToServer, NULL, 0, NULL); // 준비 완료 여부를 서버로 송신

				SceneNum = 3;
				DeleteSelBG();
				SetTimer(hWnd, 1, 1000, NULL);
				SetTimer(hWnd, 4, 1000, NULL);
				SetTimer(hWnd, 5, 10, NULL);
			}

			break;

		case 4:
			if (sqrt(pow(500 - mouse.x, 2) + pow(320 - mouse.y, 2)) <= 100) { // 다시하기 버튼을 누르면
				PlayerReady = FALSE;
				CreateThread(NULL, 0, SendToServer, NULL, 0, NULL); // 준비 완료 여부를 서버로 송신
				SceneNum = 2;
				Timer_M = 1;
				Timer_S = 0;
				ball.Reset();
				DeleteResBG();
				delete P1;
				delete P2;
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

	case WM_RBUTTONDOWN:
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);

		if (SceneNum == 2) {
			for (int i = 0; i < 2; ++i) {
				for (int j = 0; j < 5; ++j) {
					if (PtInRect(&CharSelRect[j + (i * 5)], mouse)) {
						P2Num = j + (i * 5);
					}
				}
			}
		}

		switch (P2Num) {
		case 0:
			P2 = new Alien(2);
			break;
		case 1:
			P2 = new Asura(2);
			break;
		case 2:
			P2 = new Brazil(2);
			break;
		case 3:
			P2 = new Cameroon(2);
			break;
		case 4:
			P2 = new Canada(2);
			break;
		case 5:
			P2 = new Egypt(2);
			break;
		case 6:
			P2 = new Israel(2);
			break;
		case 7:
			P2 = new Italy(2);
			break;
		case 8:
			P2 = new Korea(2);
			break;
		case 9:
			P2 = new Poland(2);
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
	if (KB[VK_LEFT])
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
	}

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

// 클라이언트 -> 서버
DWORD WINAPI SendToServer(LPVOID arg) {
	int retval;

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

	if (SceneNum == 2) {
		//플레이어 준비 상태 전송
		retval = send(sock, (char*)&PlayerReady, sizeof(bool), 0);
		if (retval == SOCKET_ERROR) err_quit("send() - PlayerReady");
		printf("[클라이언트] 플레이어 준비 상태 전송 완료: %s\n", PlayerReady ? "준비됨" : "준비 안됨");
	}

	closesocket(sock);
	return 0;
}